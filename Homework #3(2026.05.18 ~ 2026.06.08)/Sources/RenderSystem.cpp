#include "Precompiled.h"
#include "RenderSystem.h"

#include "Camera.h"
#include "ColorRGBA.h"
#include "GameObject.h"
#include "Light.h"
#include "Logger.h"
#include "Material.h"
#include "Matrix4x4.h"
#include "Mesh.h"
#include "Shader.h"

bool RenderSystem::Initialize(HWND window_)
{
	assert(window_ != nullptr);

	Logger::Info(L"Initialization started.");

	if (!CreateDevice())
	{
		Logger::Critical(L"Initialization failed: stopped while creating the device.");
		return false;
	}
	if (!CreateGraphicsRootSignature())
	{
		Logger::Critical(L"Initialization failed: stopped while creating the root signature.");
		return false;
	}
	if (!CreateCommandObjects())
	{
		Logger::Critical(L"Initialization failed: stopped while creating command objects.");
		return false;
	}
	if (!CreateSwapChain(window_))
	{
		Logger::Critical(L"Initialization failed: stopped while creating the swap chain.");
		return false;
	}
	if (!CreateRenderTargetViews())
	{
		Logger::Critical(L"Initialization failed: stopped while creating render target views.");
		return false;
	}
	if (!CreateDepthStencilView())
	{
		Logger::Critical(L"Initialization failed: stopped while creating the depth stencil view.");
		return false;
	}
	if (!CreateFence())
	{
		Logger::Critical(L"Initialization failed: stopped while creating the fence.");
		return false;
	}
	if (!CreateConstantBuffers())
	{
		Logger::Critical(L"Initialization failed: stopped while creating constant buffers.");
		return false;
	}

	WaitForGPU();
	Logger::Info(L"Initialization completed.");

	return true;
}

void RenderSystem::Release()
{
	if (commandQueue != nullptr)
	{
		WaitForGPU();
	}

	if (fenceEvent != nullptr)
	{
		::CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

	for (FrameConstantBuffer& constantBuffer : constantBuffers)
	{
		if (constantBuffer.resource != nullptr && constantBuffer.mappedData != nullptr)
		{
			constantBuffer.resource->Unmap(0, nullptr);
			constantBuffer.mappedData = nullptr;
		}

		constantBuffer.resource.Reset();
		constantBuffer.gpuAddress = 0;
		constantBuffer.currentOffset = 0;
	}

	for (FrameInstanceBuffer& instanceBuffer : instanceBuffers)
	{
		if (instanceBuffer.resource != nullptr && instanceBuffer.mappedData != nullptr)
		{
			instanceBuffer.resource->Unmap(0, nullptr);
			instanceBuffer.mappedData = nullptr;
		}

		instanceBuffer.resource.Reset();
		instanceBuffer.gpuAddress = 0;
		instanceBuffer.currentOffset = 0;
	}

	depthStencilBuffer.Reset();
	for (Microsoft::WRL::ComPtr<ID3D12Resource>& renderTarget : renderTargets)
	{
		renderTarget.Reset();
	}

	srvHeap.Reset();
	dsvHeap.Reset();
	rtvHeap.Reset();

	swapChain.Reset();
	commandList.Reset();
	for (Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& commandAllocator : commandAllocators)
	{
		commandAllocator.Reset();
	}
	commandQueue.Reset();

	graphicsRootSignature.Reset();
	fence.Reset();
	device.Reset();
	factory.Reset();

	frameIndex = 0;
	rtvDescriptorSize = 0;
	dsvDescriptorSize = 0;
	srvDescriptorSize = 0;
	fenceValues.fill(0);
	viewport = {};
	scissorRect = {};
}

void RenderSystem::PreRender()
{
	if (commandAllocators[frameIndex] == nullptr || commandList == nullptr)
	{
		Logger::Error(L"PreRender failed: command allocator or command list is not initialized.");
		return;
	}

	if (renderTargets[frameIndex] == nullptr || rtvHeap == nullptr)
	{
		Logger::Error(L"PreRender failed: current frame render target or RTV heap is missing.");
		return;
	}

	commandAllocators[frameIndex]->Reset();
	commandList->Reset(commandAllocators[frameIndex].Get(), nullptr);

	constantBuffers[frameIndex].currentOffset = 0;
	instanceBuffers[frameIndex].currentOffset = 0;
	renderRequests.clear();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = renderTargets[frameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	rtvHandle.ptr += static_cast<SIZE_T>(frameIndex) * rtvDescriptorSize;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	if (commandList != nullptr && graphicsRootSignature != nullptr)
	{
		commandList->SetGraphicsRootSignature(graphicsRootSignature.Get());
	}
	else
	{
		Logger::Error(L"PreRender failed: graphics root signature is not initialized.");
	}
}

void RenderSystem::PostRender()
{
	if (commandList == nullptr || commandQueue == nullptr)
	{
		Logger::Error(L"PostRender failed: command list or command queue is not initialized.");
		return;
	}

	if (renderTargets[frameIndex] == nullptr)
	{
		Logger::Error(L"PostRender failed: current frame render target is missing.");
		return;
	}

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = renderTargets[frameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();

	ID3D12CommandList* const commandLists[]{ commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);
}

void RenderSystem::Clear(const ColorRGBA& clearColor_)
{
	if (commandList == nullptr || rtvHeap == nullptr || dsvHeap == nullptr)
	{
		Logger::Error(L"Clear failed: command list, RTV heap, or DSV heap is not initialized.");
		return;
	}

	if (renderTargets[frameIndex] == nullptr)
	{
		Logger::Error(L"Clear failed: current frame render target is missing.");
		return;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	rtvHandle.ptr += static_cast<SIZE_T>(frameIndex) * rtvDescriptorSize;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };

	const FLOAT clearColor[]{ clearColor_.x, clearColor_.y, clearColor_.z, clearColor_.w };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void RenderSystem::Present()
{
	if (swapChain == nullptr)
	{
		Logger::Error(L"Present failed: swap chain is not initialized.");
		return;
	}

	swapChain->Present(1, 0);
	MoveToNextFrame();
}

void RenderSystem::SetCameraConstants(const CameraConstants& constants_)
{
	const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ UploadConstantData(&constants_, sizeof(constants_)) };
	if (gpuAddress != 0 && commandList != nullptr)
	{
		commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::Camera), gpuAddress);
	}
}

void RenderSystem::SetLightConstants(const LightConstants& constants_)
{
	const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ UploadConstantData(&constants_, sizeof(constants_)) };
	if (gpuAddress != 0 && commandList != nullptr)
	{
		commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::Light), gpuAddress);
	}
}

void RenderSystem::SetObjectConstants(const ObjectConstants& constants_)
{
	const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ UploadConstantData(&constants_, sizeof(constants_)) };
	if (gpuAddress != 0 && commandList != nullptr)
	{
		commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::Object), gpuAddress);
	}
}

void RenderSystem::SetMaterialConstants(const MaterialConstants& constants_)
{
	const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ UploadConstantData(&constants_, sizeof(constants_)) };
	if (gpuAddress != 0 && commandList != nullptr)
	{
		commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::Material), gpuAddress);
	}
}

void RenderSystem::SubmitRenderRequest(Mesh* mesh_, Material* material_, const Matrix4x4& worldMatrix_)
{
	if (mesh_ == nullptr || material_ == nullptr)
	{
		return;
	}

	RenderRequest& request{ renderRequests.emplace_back() };
	request.mesh = mesh_;
	request.material = material_;
	request.worldMatrix = worldMatrix_;
}

void RenderSystem::DrawMesh(Mesh* mesh_, Material* material_, const Matrix4x4& worldMatrix_)
{
	const std::array<Matrix4x4, 1> worldMatrices{ worldMatrix_ };
	DrawMeshInstanced(mesh_, material_, worldMatrices);
}

void RenderSystem::DrawMeshInstanced(Mesh* mesh_, Material* material_, std::span<const Matrix4x4> worldMatrices_)
{
	if (mesh_ == nullptr || material_ == nullptr || device == nullptr || commandList == nullptr || graphicsRootSignature == nullptr)
	{
		Logger::Error(L"DrawMeshInstanced failed: mesh, material, device, command list, or root signature is not initialized.");
		return;
	}

	if (worldMatrices_.empty())
	{
		Logger::Error(L"DrawMeshInstanced failed: instance world matrix list is empty.");
		return;
	}

	Shader* const shader{ material_->GetShader() };
	if (shader == nullptr)
	{
		Logger::Critical(L"DrawMeshInstanced failed: material has no shader.");
		return;
	}

	if (!shader->HasPipelineState() && !shader->CreatePipelineState(device.Get(), graphicsRootSignature.Get(), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT))
	{
		Logger::Critical(L"DrawMeshInstanced failed: failed to create shader PSO. shader={}", shader->GetPath());
		return;
	}

	if (!mesh_->HasGpuBuffers() && !mesh_->CreateBuffers(device.Get()))
	{
		Logger::Critical(L"DrawMeshInstanced failed: failed to create mesh GPU buffers. mesh={}", mesh_->GetPath());
		return;
	}

	const D3D12_VERTEX_BUFFER_VIEW instanceBufferView{ UploadInstanceWorldMatrices(worldMatrices_) };
	if (instanceBufferView.BufferLocation == 0 || instanceBufferView.SizeInBytes == 0)
	{
		Logger::Critical(L"DrawMeshInstanced failed: failed to upload instance buffer.");
		return;
	}

	commandList->SetPipelineState(shader->GetPipelineState());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[2]
	{
		mesh_->GetVertexBufferView(),
		instanceBufferView
	};
	commandList->IASetVertexBuffers(0, static_cast<UINT>(std::size(vertexBufferViews)), vertexBufferViews);

	if (mesh_->GetIndexCount() > 0)
	{
		const D3D12_INDEX_BUFFER_VIEW& indexBufferView{ mesh_->GetIndexBufferView() };
		commandList->IASetIndexBuffer(&indexBufferView);
		commandList->DrawIndexedInstanced(mesh_->GetIndexCount(), static_cast<UINT>(worldMatrices_.size()), 0, 0, 0);
	}
	else
	{
		commandList->DrawInstanced(static_cast<UINT>(mesh_->GetVertices().size()), static_cast<UINT>(worldMatrices_.size()), 0, 0);
	}
}

void RenderSystem::Render()
{
	if (renderRequests.empty())
	{
		return;
	}

	std::ranges::sort(renderRequests, [](const RenderRequest& lhs_, const RenderRequest& rhs_)
	{
		if (lhs_.mesh != rhs_.mesh)
		{
			return std::less<Mesh*>{}(lhs_.mesh, rhs_.mesh);
		}

		return std::less<Material*>{}(lhs_.material, rhs_.material);
	});

	for (std::size_t beginIndex{ 0 }; beginIndex < renderRequests.size();)
	{
		const RenderRequest& firstRequest{ renderRequests[beginIndex] };
		std::size_t endIndex{ beginIndex + 1 };
		while (endIndex < renderRequests.size()
			&& renderRequests[endIndex].mesh == firstRequest.mesh
			&& renderRequests[endIndex].material == firstRequest.material)
		{
			++endIndex;
		}

		MaterialConstants materialData{};
		materialData.baseColor = firstRequest.material->GetBaseColor();
		materialData.emissiveColor = firstRequest.material->GetEmissiveColor();
		materialData.metallic = firstRequest.material->GetMetallic();
		materialData.roughness = firstRequest.material->GetRoughness();
		SetMaterialConstants(materialData);

		if (endIndex - beginIndex == 1)
		{
			DrawMesh(firstRequest.mesh, firstRequest.material, firstRequest.worldMatrix);
		}
		else
		{
			batchedWorldMatrices.clear();
			batchedWorldMatrices.reserve(endIndex - beginIndex);

			for (std::size_t requestIndex{ beginIndex }; requestIndex < endIndex; ++requestIndex)
			{
				batchedWorldMatrices.emplace_back(renderRequests[requestIndex].worldMatrix);
			}

			DrawMeshInstanced(firstRequest.mesh, firstRequest.material, batchedWorldMatrices);
		}

		beginIndex = endIndex;
	}
}

ID3D12Device* RenderSystem::GetDevice() const noexcept
{
	return device.Get();
}

ID3D12GraphicsCommandList* RenderSystem::GetCommandList() const noexcept
{
	return commandList.Get();
}

const D3D12_VIEWPORT& RenderSystem::GetViewport() const noexcept
{
	return viewport;
}

bool RenderSystem::CreateDevice()
{
	UINT factoryFlags{ 0 };

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		debugController->EnableDebugLayer();
		Logger::Trace(L"D3D12 debug layer enabled.");
	}
	else
	{
		Logger::Error(L"Failed to initialize D3D12 debug controller.");
	}
#endif

	if (FAILED(::CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory))))
	{
		Logger::Critical(L"Failed to create DXGI factory.");
		return false;
	}

	if (FAILED(::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
	{
		Logger::Critical(L"Failed to create D3D12 device.");
		return false;
	}

	return true;
}

bool RenderSystem::CreateGraphicsRootSignature()
{
	if (device == nullptr)
	{
		Logger::Critical(L"Root signature creation failed: device is not initialized.");
		return false;
	}

	D3D12_ROOT_PARAMETER rootParameters[4]{};
	rootParameters[std::to_underlying(RootParameter::Camera)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[std::to_underlying(RootParameter::Camera)].Descriptor.ShaderRegister = 1;
	rootParameters[std::to_underlying(RootParameter::Camera)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[static_cast<UINT>(RootParameter::Object)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[static_cast<UINT>(RootParameter::Object)].Descriptor.ShaderRegister = 2;
	rootParameters[static_cast<UINT>(RootParameter::Object)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[static_cast<UINT>(RootParameter::Light)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[static_cast<UINT>(RootParameter::Light)].Descriptor.ShaderRegister = 4;
	rootParameters[static_cast<UINT>(RootParameter::Light)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[static_cast<UINT>(RootParameter::Material)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[static_cast<UINT>(RootParameter::Material)].Descriptor.ShaderRegister = 3;
	rootParameters[static_cast<UINT>(RootParameter::Material)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.NumParameters = static_cast<UINT>(std::size(rootParameters));
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	if (FAILED(::D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&errorBlob)))
	{
		if (errorBlob != nullptr)
		{
			const char* errorChars{ static_cast<const char*>(errorBlob->GetBufferPointer()) };
			const std::string errorText(errorChars, errorBlob->GetBufferSize());
			Logger::Critical(L"Failed to serialize graphics root signature: {}", std::wstring(errorText.begin(), errorText.end()));
		}
		else
		{
			Logger::Critical(L"Failed to serialize graphics root signature.");
		}
		return false;
	}

	if (FAILED(device->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&graphicsRootSignature))))
	{
		Logger::Critical(L"Failed to create graphics root signature.");
		return false;
	}

	return true;
}

bool RenderSystem::CreateCommandObjects()
{
	if (device == nullptr)
	{
		Logger::Critical(L"Command object creation failed: device is not initialized.");
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
	{
		Logger::Critical(L"Failed to create command queue.");
		return false;
	}

	for (uint32_t i{ 0 }; i < BackBufferCount; ++i)
	{
		if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]))))
		{
			Logger::Critical(L"Failed to create command allocator. index={}", i);
			return false;
		}
	}

	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&commandList))))
	{
		Logger::Critical(L"Failed to create command list.");
		return false;
	}

	commandList->Close();

	return true;
}

bool RenderSystem::CreateSwapChain(HWND window_)
{
	if (window_ == nullptr)
	{
		Logger::Critical(L"Swap chain creation failed: window handle is null.");
		return false;
	}

	if (factory == nullptr || commandQueue == nullptr)
	{
		Logger::Critical(L"Swap chain creation failed: factory or command queue is not initialized.");
		return false;
	}

	RECT rect;
	::GetClientRect(window_, &rect);
	const LONG width{ rect.right - rect.left };
	const LONG height{ rect.bottom - rect.top };

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = static_cast<LONG>(width);
	scissorRect.bottom = static_cast<LONG>(height);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = BackBufferCount;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	if (FAILED(factory->CreateSwapChainForHwnd(
		commandQueue.Get(), 
		window_, 
		&swapChainDesc,
		nullptr, 
		nullptr, 
		&swapChain1)))
	{
		Logger::Critical(L"Failed to create swap chain.");
		return false;
	}

	if (FAILED(swapChain1.As(&swapChain)))
	{
		Logger::Critical(L"Failed to convert swap chain to IDXGISwapChain3.");
		return false;
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	return true;
}

bool RenderSystem::CreateRenderTargetViews()
{
	if (device == nullptr || swapChain == nullptr)
	{
		Logger::Critical(L"RTV creation failed: device or swap chain is not initialized.");
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = BackBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap))))
	{
		 Logger::Critical(L"Failed to create RTV heap.");
		 return false;
	}
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (uint32_t count{ 0 }; count < BackBufferCount; ++count)
	{
		if (FAILED(swapChain->GetBuffer(count, IID_PPV_ARGS(&renderTargets[count]))))
		{
			Logger::Critical(L"Failed to get swap chain back buffer. index={}", count);
			return false;
		}

		device->CreateRenderTargetView(renderTargets[count].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += rtvDescriptorSize;
	}

	return true;
}

bool RenderSystem::CreateDepthStencilView()
{
	if (device == nullptr || swapChain == nullptr)
	{
		Logger::Critical(L"DSV creation failed: device or swap chain is not initialized.");
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap))))
	{
		Logger::Critical(L"Failed to create DSV heap.");
		return false;
	}
	dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Alignment = 0;
	depthResourceDesc.Width = static_cast<UINT64>(viewport.Width);
	depthResourceDesc.Height = static_cast<UINT>(viewport.Height);
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.MipLevels = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.SampleDesc.Quality = 0;
	depthResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthOptimizedClearValue{};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	if (FAILED(device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&depthStencilBuffer))))
	{
		Logger::Critical(L"Failed to create depth stencil buffer resource.");
		return false;
	}

	device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}

bool RenderSystem::CreateFence()
{
	if (device == nullptr)
	{
		Logger::Critical(L"Fence creation failed: device is not initialized.");
		return false;
	}

	if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
	{
		Logger::Critical(L"Failed to create fence.");
		return false;
	}

	fenceValues.fill(0);

	fenceEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		Logger::Critical(L"Failed to create fence event.");
		return false;
	}

	return true;
}

bool RenderSystem::CreateConstantBuffers()
{
	if (device == nullptr)
	{
		Logger::Critical(L"Constant buffer creation failed: device is not initialized.");
		return false;
	}

	for (FrameConstantBuffer& constantBuffer : constantBuffers)
	{
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = MaxConstantBufferSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBuffer.resource))))
		{
			Logger::Critical(L"Failed to create constant buffer resource.");
			return false;
		}

		if (FAILED(constantBuffer.resource->Map(0, nullptr, reinterpret_cast<void**>(&constantBuffer.mappedData))))
		{
			Logger::Critical(L"Failed to map constant buffer.");
			return false;
		}

		constantBuffer.gpuAddress = constantBuffer.resource->GetGPUVirtualAddress();
		constantBuffer.currentOffset = 0;
	}

	for (FrameInstanceBuffer& instanceBuffer : instanceBuffers)
	{
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = MaxInstanceBufferSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&instanceBuffer.resource))))
		{
			Logger::Critical(L"Failed to create instance buffer resource.");
			return false;
		}

		if (FAILED(instanceBuffer.resource->Map(0, nullptr, reinterpret_cast<void**>(&instanceBuffer.mappedData))))
		{
			Logger::Critical(L"Failed to map instance buffer.");
			return false;
		}

		instanceBuffer.gpuAddress = instanceBuffer.resource->GetGPUVirtualAddress();
		instanceBuffer.currentOffset = 0;
	}

	return true;
}

D3D12_VERTEX_BUFFER_VIEW RenderSystem::UploadInstanceWorldMatrices(std::span<const Matrix4x4> worldMatrices_)
{
	D3D12_VERTEX_BUFFER_VIEW bufferView{};
	if (worldMatrices_.empty())
	{
		return bufferView;
	}

	FrameInstanceBuffer& instanceBuffer{ instanceBuffers[frameIndex] };
	const UINT sizeInBytes{ static_cast<UINT>(sizeof(Matrix4x4) * worldMatrices_.size()) };
	const UINT alignedSize{ AlignBufferSize(sizeInBytes, 16) };
	if (instanceBuffer.mappedData == nullptr || instanceBuffer.currentOffset + alignedSize > MaxInstanceBufferSize)
	{
		Logger::Error(
			L"[RenderSystem] Instance buffer upload failed: buffer is not mapped or there is not enough space. requestedSize={}, currentOffset={}, maxSize={}",
			alignedSize, instanceBuffer.currentOffset, MaxInstanceBufferSize);
		return bufferView;
	}

	std::memcpy(instanceBuffer.mappedData + instanceBuffer.currentOffset, worldMatrices_.data(), sizeInBytes);

	bufferView.BufferLocation = instanceBuffer.gpuAddress + instanceBuffer.currentOffset;
	bufferView.StrideInBytes = sizeof(Matrix4x4);
	bufferView.SizeInBytes = sizeInBytes;

	instanceBuffer.currentOffset += alignedSize;
	return bufferView;
}

void RenderSystem::WaitForGPU()
{
	if (commandQueue == nullptr || fence == nullptr || fenceEvent == nullptr)
	{
		Logger::Critical(L"GPU wait failed: command queue, fence, or event is not initialized.");
		return;
	}

	const uint64_t current{ fenceValues[frameIndex] };
	commandQueue->Signal(fence.Get(), current);

	fence->SetEventOnCompletion(current, fenceEvent);
	::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	
	fenceValues[frameIndex]++;
}

void RenderSystem::MoveToNextFrame()
{
	if (commandQueue == nullptr || fence == nullptr || swapChain == nullptr)
	{
		Logger::Critical(L"Frame advance failed: command queue, fence, or swap chain is not initialized.");
		return;
	}

	const uint64_t currentFenceValue{ fenceValues[frameIndex] };
	commandQueue->Signal(fence.Get(), currentFenceValue);

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}
	
	fenceValues[frameIndex] = currentFenceValue + 1;
}

D3D12_GPU_VIRTUAL_ADDRESS RenderSystem::UploadConstantData(const void* data_, UINT sizeInBytes_)
{
	if (data_ == nullptr || sizeInBytes_ == 0)
	{
		Logger::Error(L"Constant data upload failed: data pointer is null or size is zero.");
		return 0;
	}

	FrameConstantBuffer& constantBuffer{ constantBuffers[frameIndex] };
	const UINT alignedSize{ AlignConstantBufferSize(sizeInBytes_) };
	if (constantBuffer.mappedData == nullptr || constantBuffer.currentOffset + alignedSize > MaxConstantBufferSize)
	{
		Logger::Error(
			L"[RenderSystem] Constant data upload failed: buffer is not mapped or there is not enough space. requestedSize={}, currentOffset={}, maxSize={}",
			alignedSize, constantBuffer.currentOffset, MaxConstantBufferSize);
		return 0;
	}

	std::memcpy(constantBuffer.mappedData + constantBuffer.currentOffset, data_, sizeInBytes_);

	const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ constantBuffer.gpuAddress + constantBuffer.currentOffset };
	constantBuffer.currentOffset += alignedSize;
	return gpuAddress;
}

UINT RenderSystem::AlignConstantBufferSize(UINT sizeInBytes_) noexcept
{
	return (sizeInBytes_ + 255u) & ~255u;
}

UINT RenderSystem::AlignBufferSize(UINT sizeInBytes_, UINT alignment_) noexcept
{
	return (sizeInBytes_ + (alignment_ - 1u)) & ~(alignment_ - 1u);
}
