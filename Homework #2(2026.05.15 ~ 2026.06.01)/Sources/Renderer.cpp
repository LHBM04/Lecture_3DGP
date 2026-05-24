#include "Precompiled.h"
#include "Renderer.h"

#include "Camera.h"
#include "CameraConstants.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "RendererOptions.h"
#include "RootParameterSlot.h"
#include "Shader.h"
#include "Transform.h"

namespace
{
	std::size_t Align(std::size_t value_, std::size_t alignment_) noexcept
	{
		return (value_ + alignment_ - 1u) & ~(alignment_ - 1u);
	}
}

Renderer::~Renderer()
{
	Renderer::Release();
}

bool Renderer::Initialize(const RendererOptions& options_)
{
	const bool requestedFullscreen{ options_.fullscreen };
	options = options_;
	options.fullscreen = false;
	isFullscreen = false;
	windowedWidth = std::max(1, options.width);
	windowedHeight = std::max(1, options.height);

	for (BackBuffer& backBuffer : backBuffers)
	{
		backBuffer.resource.Reset();
		backBuffer.rtv = {};
		backBuffer.state = D3D12_RESOURCE_STATE_PRESENT;
	}

	for (FrameResource& frameResource : frameResources)
	{
		frameResource.commandAllocator.Reset();
		frameResource.uploadBuffer.Reset();
		frameResource.mappedUploadBuffer = nullptr;
		frameResource.capacity = 0;
		frameResource.offset = 0;
		frameResource.fenceValue = 0;
	}

	currentBackBufferIndex = 0;
	currentFrameResourceIndex = 0;
	rtvDescriptorOffset = 0;
	dsvDescriptorOffset = 0;
	nextFenceValue = 1;
	fenceEvent = nullptr;

	CreateDevice();
	CreateCommandQueue();
	CreateFrameResources();
	CreateCommandList();
	CreateSwapChain();
	CreateDescriptorHeaps();
	CreateBackBuffers();
	CreateDepthStencilBuffer();
	CreateFence();
	if (requestedFullscreen)
	{
		SetFullscreen(true);
	}

	return nullptr != device &&
		nullptr != commandQueue &&
		nullptr != commandList &&
		nullptr != swapChain &&
		nullptr != rtvDescriptorHeap &&
		nullptr != dsvDescriptorHeap &&
		nullptr != depthStencilBuffer &&
		nullptr != fence &&
		nullptr != fenceEvent;
}

void Renderer::Release()
{
	WaitIdle();

	if (nullptr != swapChain)
	{
		(void)swapChain->SetFullscreenState(FALSE, nullptr);
	}

	for (FrameResource& frameResource : frameResources)
	{
		if (nullptr != frameResource.mappedUploadBuffer && nullptr != frameResource.uploadBuffer)
		{
			frameResource.uploadBuffer->Unmap(0, nullptr);
		}

		frameResource.commandAllocator.Reset();
		frameResource.uploadBuffer.Reset();
		frameResource.mappedUploadBuffer = nullptr;
		frameResource.capacity = 0;
		frameResource.offset = 0;
		frameResource.fenceValue = 0;
	}

	ReleaseDepthStencilBuffer();
	dsvDescriptorHeap.Reset();

	ReleaseBackBuffers();
	rtvDescriptorHeap.Reset();
	swapChain.Reset();
	commandList.Reset();
	commandQueue.Reset();
	fence.Reset();
	factory.Reset();
	device.Reset();

	if (nullptr != fenceEvent)
	{
		::CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

	drawCalls.clear();
	visibleDrawCalls.clear();
	batches.clear();
	isFullscreen = false;
}

void Renderer::BeginRender()
{
	currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
	currentFrameResourceIndex = currentBackBufferIndex;

	FrameResource& frameResource{ frameResources[currentFrameResourceIndex] };

	WaitForFrame(frameResource);
	frameResource.offset = 0;

	assert(SUCCEEDED(frameResource.commandAllocator->Reset()));
	assert(SUCCEEDED(commandList->Reset(frameResource.commandAllocator.Get(), nullptr)));

	TransitionBackBuffer(D3D12_RESOURCE_STATE_RENDER_TARGET);

	BackBuffer& backBuffer{ backBuffers[currentBackBufferIndex] };
	D3D12_CPU_DESCRIPTOR_HANDLE rtv{ backBuffer.rtv };
	D3D12_CPU_DESCRIPTOR_HANDLE dsv{ dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

	D3D12_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(options.width);
	viewport.Height = static_cast<float>(options.height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandList->RSSetViewports(1, &viewport);

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = options.width;
	scissorRect.bottom = options.height;
	commandList->RSSetScissorRects(1, &scissorRect);
}

void Renderer::Clear()
{
	BackBuffer& backBuffer{ backBuffers[currentBackBufferIndex] };
	D3D12_CPU_DESCRIPTOR_HANDLE rtv{ backBuffer.rtv };
	D3D12_CPU_DESCRIPTOR_HANDLE dsv{ dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	commandList->ClearRenderTargetView(rtv, DirectX::Colors::Blue, 0, nullptr);
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void Renderer::EndRender()
{
	TransitionBackBuffer(D3D12_RESOURCE_STATE_PRESENT);

	assert(SUCCEEDED(commandList->Close()));

	ID3D12CommandList* commandLists[]{ commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);
	assert(SUCCEEDED(swapChain->Present(options.vSync ? 1u : 0u, 0)));

	FrameResource& frameResource{ frameResources[currentFrameResourceIndex] };
	frameResource.fenceValue = SignalFence();
}

void Renderer::SetCamera(const Camera* camera_)
{
	if (nullptr == camera_)
	{
		return;
	}

	currentCameraConstants = {};

	const Vector4D& viewportRect{ camera_->GetViewport() };
	const float targetWidth{ static_cast<float>(std::max(1, options.width)) };
	const float targetHeight{ static_cast<float>(std::max(1, options.height)) };

	D3D12_VIEWPORT viewport{};
	viewport.TopLeftX = viewportRect.x * targetWidth;
	viewport.TopLeftY = viewportRect.y * targetHeight;
	viewport.Width = std::max(1.0f, viewportRect.z * targetWidth);
	viewport.Height = std::max(1.0f, viewportRect.w * targetHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandList->RSSetViewports(1, &viewport);

	D3D12_RECT scissorRect{};
	scissorRect.left = static_cast<LONG>(viewport.TopLeftX);
	scissorRect.top = static_cast<LONG>(viewport.TopLeftY);
	scissorRect.right = static_cast<LONG>(viewport.TopLeftX + viewport.Width);
	scissorRect.bottom = static_cast<LONG>(viewport.TopLeftY + viewport.Height);
	commandList->RSSetScissorRects(1, &scissorRect);

	const float aspect{ viewport.Width / viewport.Height };

	currentCameraConstants.view = camera_->GetViewMatrix();
	currentCameraConstants.projection = camera_->GetProjectionMatrix(aspect);
	currentCameraConstants.viewProjection = camera_->GetViewProjectionMatrix(aspect);
}

void Renderer::BindPipeline(const Shader& shader_)
{
	currentDrawCall.pipelineId = shader_.GetPipelineId();
	currentDrawCall.pipelineState = const_cast<ID3D12PipelineState*>(shader_.GetPipelineState());
	currentDrawCall.graphicsRootSignature = const_cast<ID3D12RootSignature*>(shader_.GetGraphicsRootSignature());
	currentDrawCall.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // Default, adjust if your Shader has topology
}

void Renderer::BindMaterial(const Material& material_)
{
	currentDrawCall.materialId = material_.GetId();
	currentDrawCall.materialDescriptorTable = material_.GetDescriptorTable();
	currentDrawCall.materialColor = material_.GetColor();
}

void Renderer::BindMesh(const Mesh& mesh_)
{
	currentDrawCall.meshId = mesh_.GetId();
	currentDrawCall.vertexBufferView = mesh_.GetVertexBufferView();

	if (mesh_.HasIndexBuffer())
	{
		currentDrawCall.indexBufferView = mesh_.GetIndexBufferView();
		currentDrawCall.hasIndexBuffer = true;
		currentDrawCall.indexed = true;

		currentDrawCall.indexCount = mesh_.GetIndexCount();
		currentDrawCall.startIndexLocation = 0; // Tick if mesh adds this
		currentDrawCall.baseVertexLocation = 0; // Tick if mesh adds this
	}
	else
	{
		currentDrawCall.indexBufferView = {};
		currentDrawCall.hasIndexBuffer = false;
		currentDrawCall.indexed = false;

		currentDrawCall.vertexCount = mesh_.GetVertexCount();
		currentDrawCall.startVertexLocation = 0; // Tick if mesh adds this
	}
}

void Renderer::SetObject(const GameObject* gameObject_)
{
	currentDrawCall.worldTransform = gameObject_->GetComponent<Transform>()->GetWorldMatrix();
}

void Renderer::Render()
{
	if (!IsValidDrawCall(currentDrawCall))
	{
		return;
	}

	DrawCall drawCall{ currentDrawCall };
	drawCall.instanceCount = 1;
	drawCall.startInstanceLocation = 0;
	drawCall.sortKey = BuildSortKey(drawCall);

	drawCalls.push_back(drawCall);
}

void Renderer::Flush()
{
	visibleDrawCalls.clear();
	batches.clear();

	BuildVisibleDrawCalls();
	SortDrawCalls();
	BuildBatches();
	ExecuteBatches();

	drawCalls.clear();
	visibleDrawCalls.clear();
	batches.clear();
}

ID3D12Device* Renderer::GetDevice() const noexcept
{
	return device.Get();
}

bool Renderer::IsFullscreen() const noexcept
{
	return isFullscreen;
}

int Renderer::GetWidth() const noexcept
{
	return options.width;
}

int Renderer::GetHeight() const noexcept
{
	return options.height;
}

void Renderer::WaitIdle()
{
	for (FrameResource& frameResource : frameResources)
	{
		WaitForFrame(frameResource);
	}
}

void Renderer::SetFullscreen(bool fullscreen_)
{
	if (nullptr == swapChain)
	{
		return;
	}

	if (fullscreen_ == isFullscreen)
	{
		return;
	}

	WaitIdle();

	if (!isFullscreen)
	{
		windowedWidth = std::max(1, options.width);
		windowedHeight = std::max(1, options.height);
	}

	ReleaseDepthStencilBuffer();
	ReleaseBackBuffers();

	int targetWidth{ windowedWidth };
	int targetHeight{ windowedHeight };

	if (fullscreen_)
	{
		Microsoft::WRL::ComPtr<IDXGIOutput> output;
		if (SUCCEEDED(swapChain->GetContainingOutput(&output)))
		{
			DXGI_MODE_DESC desiredMode{};
			desiredMode.Width = static_cast<UINT>(options.width);
			desiredMode.Height = static_cast<UINT>(options.height);
			desiredMode.RefreshRate.Numerator = 0;
			desiredMode.RefreshRate.Denominator = 1;
			desiredMode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desiredMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			desiredMode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			DXGI_MODE_DESC closestMode{};
			if (SUCCEEDED(output->FindClosestMatchingMode(&desiredMode, &closestMode, device.Get())))
			{
				(void)swapChain->ResizeTarget(&closestMode);
				targetWidth = static_cast<int>(closestMode.Width);
				targetHeight = static_cast<int>(closestMode.Height);
			}

			const HRESULT result{ swapChain->SetFullscreenState(TRUE, output.Get()) };
			if (FAILED(result))
			{
				fullscreen_ = false;
				targetWidth = windowedWidth;
				targetHeight = windowedHeight;
			}
		}
		else
		{
			fullscreen_ = false;
		}
	}
	else
	{
		(void)swapChain->SetFullscreenState(FALSE, nullptr);
	}

	const HRESULT resizeResult
	{
		swapChain->ResizeBuffers(
			static_cast<UINT>(FrameCount),
			static_cast<UINT>(targetWidth),
			static_cast<UINT>(targetHeight),
			DXGI_FORMAT_R8G8B8A8_UNORM,
			0)
	};

	assert(SUCCEEDED(resizeResult));

	options.width = targetWidth;
	options.height = targetHeight;
	options.fullscreen = fullscreen_;
	isFullscreen = fullscreen_;

	currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
	CreateBackBuffers();
	CreateDepthStencilBuffer();
}

void Renderer::ToggleFullscreen()
{
	SetFullscreen(!isFullscreen);
}

void Renderer::CreateDevice()
{
	UINT factoryFlags{ 0 };

#if defined(_DEBUG)
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	assert(SUCCEEDED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory))));
	assert(SUCCEEDED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))));
}

void Renderer::CreateCommandQueue()
{
	assert(nullptr != device);

	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	assert(SUCCEEDED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue))));
}

void Renderer::CreateFrameResources()
{
	assert(nullptr != device);

	for (FrameResource& frameResource : frameResources)
	{
		assert(SUCCEEDED(device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&frameResource.commandAllocator))));

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDescription{};
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Width = UploadBufferSize;
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(SUCCEEDED(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescription,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&frameResource.uploadBuffer))));

		assert(SUCCEEDED(frameResource.uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&frameResource.mappedUploadBuffer))));

		frameResource.capacity = UploadBufferSize;
		frameResource.offset = 0;
		frameResource.fenceValue = 0;
	}
}

void Renderer::CreateCommandList()
{
	assert(nullptr != device);
	assert(nullptr != frameResources[0].commandAllocator);

	assert(SUCCEEDED(device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		frameResources[0].commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList))));

	assert(SUCCEEDED(commandList->Close()));
}

void Renderer::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.Width = options.width;
	desc.Height = options.height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = FrameCount;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	assert(SUCCEEDED(factory->CreateSwapChainForHwnd(commandQueue.Get(), options.window, &desc, nullptr, nullptr, &swapChain1)));
	assert(SUCCEEDED(swapChain1.As(&swapChain)));

	assert(SUCCEEDED(factory->MakeWindowAssociation(options.window, DXGI_MWA_NO_ALT_ENTER)));

	currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
}

void Renderer::CreateDescriptorHeaps()
{
	assert(nullptr != device);

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc{};
	rtvDesc.NumDescriptors = FrameCount;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	assert(SUCCEEDED(device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&rtvDescriptorHeap))));

	rtvDescriptorOffset = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC dsvDesc{};
	dsvDesc.NumDescriptors = 1;
	dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	assert(SUCCEEDED(device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&dsvDescriptorHeap))));
	dsvDescriptorOffset = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void Renderer::CreateBackBuffers()
{
	assert(nullptr != device);
	assert(nullptr != swapChain);
	assert(nullptr != rtvDescriptorHeap);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv{ rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT index{ 0 }; index < FrameCount; ++index)
	{
		assert(SUCCEEDED(swapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffers[index].resource))));

		backBuffers[index].rtv = rtv;
		backBuffers[index].state = D3D12_RESOURCE_STATE_PRESENT;
		device->CreateRenderTargetView(backBuffers[index].resource.Get(), nullptr, rtv);
		rtv.ptr += rtvDescriptorOffset;
	}
}

void Renderer::CreateDepthStencilBuffer()
{
	assert(nullptr != device);
	assert(nullptr != dsvDescriptorHeap);

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = static_cast<UINT64>(options.width);
	desc.Height = static_cast<UINT>(options.height);
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	assert(SUCCEEDED(device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&depthStencilBuffer))));

	device->CreateDepthStencilView(
		depthStencilBuffer.Get(),
		nullptr,
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void Renderer::ReleaseBackBuffers()
{
	for (BackBuffer& backBuffer : backBuffers)
	{
		backBuffer.resource.Reset();
		backBuffer.rtv = {};
		backBuffer.state = D3D12_RESOURCE_STATE_PRESENT;
	}
}

void Renderer::ReleaseDepthStencilBuffer()
{
	depthStencilBuffer.Reset();
}

void Renderer::CreateFence()
{
	assert(nullptr != device);
	assert(SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))));

	fenceEvent = ::CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	assert(nullptr != fenceEvent);
	nextFenceValue = 1;
}

void Renderer::ApplyFullscreenState()
{
	SetFullscreen(options.fullscreen);
}

void Renderer::TransitionBackBuffer(D3D12_RESOURCE_STATES state_)
{
	BackBuffer& current{ backBuffers[currentBackBufferIndex] };
	if (state_ == current.state)
	{
		return;
	}

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = current.resource.Get();
	barrier.Transition.StateBefore = current.state;
	barrier.Transition.StateAfter = state_;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier);
	current.state = state_;
}

void Renderer::BindCameraConstants()
{
	const D3D12_GPU_VIRTUAL_ADDRESS cameraAddress{ UploadConstantData(&currentCameraConstants, sizeof(CameraConstants)) };
	if (0 != cameraAddress)
	{
		commandList->SetGraphicsRootConstantBufferView((UINT)RootParameterSlot::Camera, cameraAddress);
	}
}

void Renderer::BuildVisibleDrawCalls()
{
	visibleDrawCalls.clear();
	visibleDrawCalls.reserve(drawCalls.size());

	for (const DrawCall& drawCall : drawCalls)
	{
		visibleDrawCalls.push_back(drawCall);
	}
}

void Renderer::SortDrawCalls()
{
	std::ranges::sort(visibleDrawCalls, [](const DrawCall& a_, const DrawCall& b_) {
			return a_.sortKey < b_.sortKey;
		});
}

void Renderer::BuildBatches()
{
	batches.clear();

	Batch currentBatch{};
	const DrawCall* previous{ nullptr };

	for (const DrawCall& drawCall : visibleDrawCalls)
	{
		const bool newBatch{ nullptr == previous || !CanBatchTogether(*previous, drawCall) };
		if (newBatch)
		{
			if (!currentBatch.instances.empty())
			{
				batches.push_back(std::move(currentBatch));
				currentBatch = {};
			}

			currentBatch.baseCall = drawCall;
		}

		InstanceData instance{};
		instance.worldTransform = drawCall.worldTransform;
		currentBatch.instances.push_back(instance);

		previous = &drawCall;
	}

	if (!currentBatch.instances.empty())
	{
		batches.push_back(std::move(currentBatch));
		currentBatch = {};
	}
}

bool Renderer::CanBatchTogether(const DrawCall& a_, const DrawCall& b_) const noexcept
{
	return a_.pipelineState == b_.pipelineState &&
		a_.graphicsRootSignature == b_.graphicsRootSignature &&
		a_.primitiveTopology == b_.primitiveTopology &&
		a_.meshId == b_.meshId &&
		a_.materialId == b_.materialId &&
		a_.pipelineId == b_.pipelineId &&
		a_.materialDescriptorTable.ptr == b_.materialDescriptorTable.ptr &&
		a_.indexed == b_.indexed &&
		a_.hasIndexBuffer == b_.hasIndexBuffer &&
		a_.vertexCount == b_.vertexCount &&
		a_.startVertexLocation == b_.startVertexLocation &&
		a_.indexCount == b_.indexCount &&
		a_.startIndexLocation == b_.startIndexLocation &&
		a_.baseVertexLocation == b_.baseVertexLocation;
}

void Renderer::ExecuteBatches()
{
	if (batches.empty())
	{
		return;
	}

	ID3D12PipelineState* currentPipelineState{ nullptr };
	ID3D12RootSignature* currentRootSignature{ nullptr };
	ColorRGBA currentMaterialColor{ -1.0f, -1.0f, -1.0f, -1.0f };

	for (const Batch& batch : batches)
	{
		const DrawCall& drawCall{ batch.baseCall };

		if (drawCall.pipelineState != currentPipelineState)
		{
			commandList->SetPipelineState(drawCall.pipelineState);
			currentPipelineState = drawCall.pipelineState;
		}

		if (drawCall.graphicsRootSignature != currentRootSignature)
		{
			commandList->SetGraphicsRootSignature(drawCall.graphicsRootSignature);
			currentRootSignature = drawCall.graphicsRootSignature;

			BindCameraConstants();

			const D3D12_GPU_VIRTUAL_ADDRESS materialAddress{ UploadConstantData(&drawCall.materialColor, sizeof(ColorRGBA)) };
			if (0 != materialAddress)
			{
				commandList->SetGraphicsRootConstantBufferView((UINT)RootParameterSlot::Material, materialAddress);
			}
			currentMaterialColor = drawCall.materialColor;
		}
		else
		{
			if (drawCall.materialColor.x != currentMaterialColor.x ||
				drawCall.materialColor.y != currentMaterialColor.y ||
				drawCall.materialColor.z != currentMaterialColor.z ||
				drawCall.materialColor.w != currentMaterialColor.w)
			{
				const D3D12_GPU_VIRTUAL_ADDRESS materialAddress{ UploadConstantData(&drawCall.materialColor, sizeof(ColorRGBA)) };
				if (0 != materialAddress)
				{
					commandList->SetGraphicsRootConstantBufferView((UINT)RootParameterSlot::Material, materialAddress);
				}

				currentMaterialColor = drawCall.materialColor;
			}
		}

		ExecuteBatch(batch);
	}
}

void Renderer::ExecuteBatch(const Batch& batch_)
{
	if (batch_.instances.empty()) [[unlikely]]
	{
		return;
	}

	const DrawCall& drawCall{ batch_.baseCall };

	commandList->IASetPrimitiveTopology(drawCall.primitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW instanceBufferView{ UploadInstanceData(batch_.instances) };
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[2]{ drawCall.vertexBufferView, instanceBufferView };
	commandList->IASetVertexBuffers(0, 2, vertexBufferViews);

	if (drawCall.indexed)
	{
		commandList->IASetIndexBuffer(&drawCall.indexBufferView);
		commandList->DrawIndexedInstanced(
			drawCall.indexCount,
			static_cast<UINT>(batch_.instances.size()),
			drawCall.startIndexLocation,
			drawCall.baseVertexLocation,
			0);
	}
	else
	{
		commandList->DrawInstanced(
			drawCall.vertexCount,
			static_cast<UINT>(batch_.instances.size()),
			drawCall.startVertexLocation,
			0);
	}
}

bool Renderer::IsValidDrawCall(const DrawCall& drawCall_) const noexcept
{
	if (nullptr == drawCall_.pipelineState)
	{
		return false;
	}

	if (nullptr == drawCall_.graphicsRootSignature)
	{
		return false;
	}

	if (0 == drawCall_.vertexBufferView.BufferLocation)
	{
		return false;
	}

	if (drawCall_.indexed && 0 == drawCall_.indexBufferView.BufferLocation)
	{
		return false;
	}

	return true;
}

uint64_t Renderer::BuildSortKey(const DrawCall& drawCall_) const noexcept
{
	return ((drawCall_.pipelineId & 0xFFFFull) << 48) |
		((drawCall_.materialId & 0xFFFFFFull) << 24) |
		(drawCall_.meshId & 0xFFFFFFull);
}



void* Renderer::AllocateUploadMemory(std::size_t sizeInBytes_, std::size_t alignment_, D3D12_GPU_VIRTUAL_ADDRESS& outGpuAddress_)
{
	FrameResource& frameResource{ frameResources[currentFrameResourceIndex] };
	const std::size_t alignedOffset{ Align(frameResource.offset, alignment_) };
	const std::size_t nextOffset{ alignedOffset + sizeInBytes_ };

	assert(nextOffset <= frameResource.capacity);
	if (nullptr == frameResource.mappedUploadBuffer || nextOffset > frameResource.capacity)
	{
		outGpuAddress_ = 0;
		return nullptr;
	}

	outGpuAddress_ = frameResource.uploadBuffer->GetGPUVirtualAddress() + alignedOffset;
	frameResource.offset = nextOffset;
	return frameResource.mappedUploadBuffer + alignedOffset;
}

D3D12_GPU_VIRTUAL_ADDRESS Renderer::UploadConstantData(const void* data_, std::size_t sizeInBytes_)
{
	const std::size_t alignedSize{ Align(sizeInBytes_, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT) };
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ 0 };
	void* cpuAddress{ AllocateUploadMemory(alignedSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, gpuAddress) };
	if (nullptr == cpuAddress)
	{
		return 0;
	}

	std::memcpy(cpuAddress, data_, sizeInBytes_);
	return gpuAddress;
}

D3D12_VERTEX_BUFFER_VIEW Renderer::UploadInstanceData(std::span<const InstanceData> instances_)
{
	D3D12_VERTEX_BUFFER_VIEW view{};
	if (instances_.empty())
	{
		return view;
	}

	const std::size_t byteSize{ sizeof(InstanceData) * instances_.size() };
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ 0 };
	void* cpuAddress{ AllocateUploadMemory(byteSize, alignof(InstanceData), gpuAddress) };
	if (nullptr == cpuAddress)
	{
		return view;
	}

	std::memcpy(cpuAddress, instances_.data(), byteSize);

	view.BufferLocation = gpuAddress;
	view.SizeInBytes = static_cast<UINT>(byteSize);
	view.StrideInBytes = sizeof(InstanceData);
	return view;
}

UINT64 Renderer::SignalFence()
{
	const UINT64 fenceValue{ nextFenceValue++ };
	if (FAILED(commandQueue->Signal(fence.Get(), fenceValue)))
	{
		return 0;
	}

	return fenceValue;
}

void Renderer::WaitForFrame(FrameResource& frame_)
{
	if (nullptr == fence || nullptr == fenceEvent || 0 == frame_.fenceValue)
	{
		return;
	}

	if (fence->GetCompletedValue() >= frame_.fenceValue)
	{
		frame_.fenceValue = 0;
		return;
	}

	if (SUCCEEDED(fence->SetEventOnCompletion(frame_.fenceValue, fenceEvent)))
	{
		::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	frame_.fenceValue = 0;
}
