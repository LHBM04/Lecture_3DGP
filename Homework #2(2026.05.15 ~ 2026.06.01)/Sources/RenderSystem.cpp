#include "Precompiled.h"
#include "RenderSystem.h"

#include <algorithm>

#include "Camera.h"
#include "GameObject.h"
#include "Light.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Transform.h"
#include "ResourceSystem.h"

std::expected<void, std::wstring> RenderSystem::Initialize(HWND window_)
{
	if (auto res{ CreateDevice() }; !res)
	{
		return res;
	}
	if (auto res{ CreateCommandQueue() }; !res)
	{
		return res;
	}
	if (auto res{ CreateSwapChain(window_) }; !res)
	{
		return res;
	}
	if (auto res{ CreateDescriptorHeaps() }; !res)
	{
		return res;
	}
	if (auto res{ CreateCommandList() }; !res)
	{
		return res;
	}
	if (auto res{ CreateSyncObjects() }; !res)
	{
		return res;
	}
	if (auto res{ CreateConstantBuffer() }; !res)
	{
		return res;
	}
	if (auto res{ CreateRootSignature() }; !res)
	{
		return res;
	}
	if (auto res{ CreatePipelineState() }; !res)
	{
		return res;
	}

	WaitForGpu();
	return {};
}

void RenderSystem::Release()
{
	if (commandQueue)
	{
		WaitForGpu();
	}

	if (constantBuffer)
	{
		constantBuffer->Unmap(0, nullptr);
		mappedConstantData = nullptr;
	}

	if (fenceEvent != nullptr)
	{
		CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}
}

void RenderSystem::BeginFrame()
{
	assert(commandAllocators[frameIndex] != nullptr);
	assert(commandList != nullptr);

	if (FAILED(commandAllocators[frameIndex]->Reset()))
	{
		return;
	}
	if (FAILED(commandList->Reset(commandAllocators[frameIndex].Get(), pipelineState.Get())))
	{
		return;
	}

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = backBuffers[frameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList->ResourceBarrier(1, &barrier);

	// Bind default back buffer
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	rtvHandle.ptr += frameIndex * rtvDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };
	
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// Default clear
	float clearColor[]{ 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	if (rootSignature)
	{
		SetGraphicsRootSignature(rootSignature.Get());
	}

	constantBufferOffset = frameIndex * MaxConstantBufferSize;
}

void RenderSystem::EndFrame()
{
	assert(commandList != nullptr);
	assert(commandQueue != nullptr);

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = backBuffers[frameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	commandList->ResourceBarrier(1, &barrier);

	if (FAILED(commandList->Close()))
	{
		return;
	}

	ID3D12CommandList* ppCommandLists[]{ commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void RenderSystem::Present()
{
	assert(swapChain != nullptr);

	if (FAILED(swapChain->Present(1, 0)))
	{
		return;
	}
	MoveToNextFrame();
}

void RenderSystem::SetCamera(Camera* camera_)
{
	assert(commandList != nullptr);
	assert(camera_ != nullptr);

	camera = camera_;

	// 1. Viewport & Scissor
	const Vector4D& rect{ camera->GetViewport() };
	D3D12_VIEWPORT vp{};
	vp.TopLeftX = rect.x * width;
	vp.TopLeftY = rect.y * height;
	vp.Width = rect.z * width;
	vp.Height = rect.w * height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	D3D12_RECT scissor{};
	scissor.left = static_cast<LONG>(vp.TopLeftX);
	scissor.top = static_cast<LONG>(vp.TopLeftY);
	scissor.right = static_cast<LONG>(vp.TopLeftX + vp.Width);
	scissor.bottom = static_cast<LONG>(vp.TopLeftY + vp.Height);

	commandList->RSSetViewports(1, &vp);
	commandList->RSSetScissorRects(1, &scissor);

	camera_->UpdateFrustum();

	// 2. Clear if requested
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	rtvHandle.ptr += frameIndex * rtvDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };

	if (camera->GetClearMode() == Camera::ClearType::SolidColor)
	{
		const ColorRGBA& color = camera->GetClearColor();
		float clearColor[]{ color.x, color.y, color.z, color.w };
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	if (camera->GetClearMode() != Camera::ClearType::Nothing)
	{
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}

	// 3. Camera Constants (Slot 0)
	CameraConstants data{};
	data.viewMatrix = camera->GetViewMatrix();
	data.projectionMatrix = camera->GetProjectionMatrix();
	
	SetGraphicsRootConstantBufferView(0, UploadConstantsData(data));
}

void RenderSystem::SetLights(std::span<Light*> lights_)
{
	assert(commandList != nullptr);

	LightConstants data{};
	data.ambientColor = Vector4D{ 0.1f, 0.1f, 0.1f, 1.0f };

	if (camera)
	{
		if (auto* transform{ camera->GetOwner()->GetComponent<Transform>() })
		{
			data.cameraPosition = transform->GetWorldPosition();
		}
	}

	uint32_t activeCount{ 0 };
	for (Light* light : lights_)
	{
		if (!light || !light->GetOwner()->IsActive())
		{
			continue;
		}
		if (activeCount >= MaxLights)
		{
			break;
		}

		const ColorRGBA& lightColor{ light->GetColor() };
		data.lights[activeCount] = Vector4D{ lightColor.x, lightColor.y, lightColor.z, light->GetIntensity() };
		
		if (Transform* transform{ light->GetOwner()->GetComponent<Transform>() })
		{
			Vector3D forward = transform->GetWorldMatrix().GetForward();
			data.lightDirs[activeCount] = Vector4D{ forward.x, forward.y, forward.z, 1.0f };
		}
		
		activeCount++;
	}
	data.activeLightCount = activeCount;

	SetGraphicsRootConstantBufferView(1, UploadConstantsData(data));
}

void RenderSystem::SetPipelineState(ID3D12PipelineState* pipelineState_)
{
	assert(commandList != nullptr);
	commandList->SetPipelineState(pipelineState_);
}

void RenderSystem::SetGraphicsRootSignature(ID3D12RootSignature* rootSignature_)
{
	assert(commandList != nullptr);
	commandList->SetGraphicsRootSignature(rootSignature_);
}

void RenderSystem::SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vbv_)
{
	assert(commandList != nullptr);
	commandList->IASetVertexBuffers(0, 1, &vbv_);
}

void RenderSystem::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& ibv_)
{
	assert(commandList != nullptr);
	commandList->IASetIndexBuffer(&ibv_);
}

void RenderSystem::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology_)
{
	assert(commandList != nullptr);
	commandList->IASetPrimitiveTopology(topology_);
}

void RenderSystem::SetGraphicsRootConstantBufferView(UINT rootParameterIndex_, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress_)
{
	assert(commandList != nullptr);
	commandList->SetGraphicsRootConstantBufferView(rootParameterIndex_, gpuAddress_);
}

void RenderSystem::SetGraphicsRootDescriptorTable(UINT rootParameterIndex_, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor_)
{
	assert(commandList != nullptr);
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex_, baseDescriptor_);
}

void RenderSystem::DrawInstanced(UINT vertexCountPerInstance_, UINT instanceCount_, UINT startVertexLocation_, UINT startInstanceLocation_)
{
	assert(commandList != nullptr);
	commandList->DrawInstanced(vertexCountPerInstance_, instanceCount_, startVertexLocation_, startInstanceLocation_);
}

void RenderSystem::DrawIndexedInstanced(UINT indexCountPerInstance_, UINT instanceCount_, UINT startIndexLocation_, INT baseVertexLocation_, UINT startInstanceLocation_)
{
	assert(commandList != nullptr);
	commandList->DrawIndexedInstanced(indexCountPerInstance_, instanceCount_, startIndexLocation_, baseVertexLocation_, startInstanceLocation_);
}

void RenderSystem::SetObjectConstants(const ObjectConstants& data_)
{
	SetGraphicsRootConstantBufferView(2, UploadConstantsData(data_));
}

void RenderSystem::SetMaterialConstants(const MaterialConstants& data_)
{
	SetGraphicsRootConstantBufferView(3, UploadConstantsData(data_));
}

std::expected<void, std::wstring> RenderSystem::CreateGBuffers()
{
	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateDevice()
{
	UINT factoryFlags{ 0 };
#if defined(_DEBUG)
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory))))
	{
		return std::unexpected{ L"Failed to create DXGI Factory." };
	}

	if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
	{
		return std::unexpected{ L"Failed to create D3D12 Device." };
	}

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
	{
		return std::unexpected{ L"Failed to create Command Queue." };
	}
	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateSwapChain(HWND window_)
{
	RECT rect{};
	::GetClientRect(window_, &rect);
	width = static_cast<UINT>(rect.right - rect.left);
	height = static_cast<UINT>(rect.bottom - rect.top);

	viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	scissorRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	if (FAILED(factory->CreateSwapChainForHwnd(commandQueue.Get(), window_, &swapChainDesc, nullptr, nullptr, &swapChain1)))
	{
		return std::unexpected{ L"Failed to create Swap Chain." };
	}

	if (FAILED(swapChain1.As(&swapChain)))
	{
		return std::unexpected{ L"Failed to cast Swap Chain." };
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateDescriptorHeaps()
{
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap))))
		{
			return std::unexpected{ L"Failed to create RTV Descriptor Heap." };
		}

		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };

		for (UINT n{ 0 }; n < FrameCount; n++)
		{
			if (FAILED(swapChain->GetBuffer(n, IID_PPV_ARGS(&backBuffers[n]))))
			{
				return std::unexpected{ L"Failed to get Swap Chain Buffer." };
			}
			device->CreateRenderTargetView(backBuffers[n].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap))))
		{
			return std::unexpected{ L"Failed to create DSV Descriptor Heap." };
		}

		dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		D3D12_RESOURCE_DESC depthResourceDesc{};
		depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthResourceDesc.Alignment = 0;
		depthResourceDesc.Width = width;
		depthResourceDesc.Height = height;
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

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

		if (FAILED(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&depthResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&depthStencilBuffer))))
		{
			return std::unexpected{ L"Failed to create Depth Stencil Buffer." };
		}

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateCommandList()
{
	for (std::size_t count{ 0 }; count < FrameCount; count++)
	{
		if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[count]))))
		{
			return std::unexpected{ L"Failed to create Command Allocator." };
		}
	}

	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), nullptr, IID_PPV_ARGS(&commandList))))
	{
		return std::unexpected{ L"Failed to create Command List." };
	}

	if (FAILED(commandList->Close()))
	{
		return std::unexpected{ L"Failed to close Command List." };
	}

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateSyncObjects()
{
	if (FAILED(device->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
	{
		return std::unexpected{ L"Failed to create Fence." };
	}
	fenceValues[frameIndex]++;

	fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		return std::unexpected{ L"Failed to create Fence Event." };
	}

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateConstantBuffer()
{
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC bufferDesc{};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = MaxConstantBufferSize * FrameCount;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	if (FAILED(device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constantBuffer))))
	{
		return std::unexpected{ L"Failed to create Constant Buffer." };
	}

	if (FAILED(constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedConstantData))))
	{
		return std::unexpected{ L"Failed to map Constant Buffer." };
	}

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateRootSignature()
{
	D3D12_ROOT_PARAMETER rootParameters[4]{};

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	rootParameters[0].Descriptor.RegisterSpace = 0;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor.ShaderRegister = 1;
	rootParameters[1].Descriptor.RegisterSpace = 0;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].Descriptor.ShaderRegister = 2;
	rootParameters[2].Descriptor.RegisterSpace = 0;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].Descriptor.ShaderRegister = 3;
	rootParameters[3].Descriptor.RegisterSpace = 0;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.NumParameters = _countof(rootParameters);
	rootSigDesc.pParameters = rootParameters;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;

	if (FAILED(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
	{
		return std::unexpected{ L"Failed to serialize Root Signature." };
	}

	if (FAILED(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature))))
	{
		return std::unexpected{ L"Failed to create Root Signature." };
	}

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreatePipelineState()
{
	auto* shader{ ResourceSystem::GetInstance().GetOrLoadResource<Shader>(L"Resources/Shaders/GameObject.hlsl") };
	if (!shader)
	{
		return std::unexpected{ L"Failed to load GameObject shader." };
	}

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = { shader->GetVSBlob()->GetBufferPointer(), shader->GetVSBlob()->GetBufferSize() };
	psoDesc.PS = { shader->GetPSBlob()->GetBufferPointer(), shader->GetPSBlob()->GetBufferSize() };
	
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	psoDesc.DepthStencilState.DepthEnable = TRUE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;

	if (FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState))))
	{
		return std::unexpected{ L"Failed to create Geometry Pipeline State." };
	}

	return {};
}

void RenderSystem::WaitForGpu()
{
	assert(commandQueue != nullptr);
	assert(fence != nullptr);

	if (FAILED(commandQueue->Signal(fence.Get(), fenceValues[frameIndex])))
	{
		return;
	}
	if (FAILED(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent)))
	{
		return;
	}
	WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	fenceValues[frameIndex]++;
}

void RenderSystem::MoveToNextFrame()
{
	assert(commandQueue != nullptr);
	assert(swapChain != nullptr);
	assert(fence != nullptr);

	const UINT64 currentFenceValue{ fenceValues[frameIndex] };
	if (FAILED(commandQueue->Signal(fence.Get(), currentFenceValue)))
	{
		return;
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		if (FAILED(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent)))
		{
			return;
		}
		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}
	fenceValues[frameIndex] = currentFenceValue + 1;
}
