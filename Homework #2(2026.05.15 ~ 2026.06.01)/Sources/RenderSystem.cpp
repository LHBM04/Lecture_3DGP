#include "Precompiled.h"
#include "RenderSystem.h"

#include "Camera.h"
#include "GameObject.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "ResourceSystem.h"
#include "Shader.h"
#include "Transform.h"

std::expected<void, std::wstring> RenderSystem::Initialize(HWND hWnd_)
{
	const std::expected<void, std::wstring> deviceResult{ CreateDevice() };
	if (deviceResult.has_value() == false)
	{
		return deviceResult;
	}

	const std::expected<void, std::wstring> commandResult{ CreateCommandObjects() };
	if (commandResult.has_value() == false)
	{
		return commandResult;
	}

	const std::expected<void, std::wstring> swapChainResult{ CreateSwapChain(hWnd_) };
	if (swapChainResult.has_value() == false)
	{
		return swapChainResult;
	}

	const std::expected<void, std::wstring> heapResult{ CreateDescriptorHeaps() };
	if (heapResult.has_value() == false)
	{
		return heapResult;
	}

	const std::expected<void, std::wstring> gbufferResult{ CreateGBuffers() };
	if (gbufferResult.has_value() == false)
	{
		return gbufferResult;
	}

	const std::expected<void, std::wstring> rootSigResult{ CreateRootSignature() };
	if (rootSigResult.has_value() == false)
	{
		return rootSigResult;
	}

	const std::expected<void, std::wstring> psoResult{ CreatePipelineStates() };
	if (psoResult.has_value() == false)
	{
		return psoResult;
	}

	const std::expected<void, std::wstring> cbResult{ CreateConstantBuffer() };
	if (cbResult.has_value() == false)
	{
		return cbResult;
	}

	const std::expected<void, std::wstring> syncResult{ CreateSyncObjects() };
	if (syncResult.has_value() == false)
	{
		return syncResult;
	}

	WaitForGpu();
	return {};
}

void RenderSystem::Release()
{
	if (commandQueue != nullptr)
	{
		WaitForGpu();
	}

	if (mappedConstantData != nullptr)
	{
		constantBuffer->Unmap(0, nullptr);
		mappedConstantData = nullptr;
	}
	if (mappedInstanceData != nullptr)
	{
		instanceUploadBuffer->Unmap(0, nullptr);
		mappedInstanceData = nullptr;
	}

	if (fenceEvent != nullptr)
	{
		CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}
}

bool RenderSystem::BeginFrame()
{
	if (commandAllocators[frameIndex] == nullptr || commandList == nullptr || pipelineState == nullptr)
	{
		return false;
	}

	if (FAILED(commandAllocators[frameIndex]->Reset()))
	{
		return false;
	}
	if (FAILED(commandList->Reset(commandAllocators[frameIndex].Get(), pipelineState.Get())))
	{
		return false;
	}

	constantBufferOffset = 0;
	instanceBufferOffset = frameIndex * (instanceBufferCapacity / SwapChainBufferCount);

	if (gbufferAlbedo == nullptr || gbufferNormal == nullptr)
	{
		return false;
	}

	D3D12_RESOURCE_BARRIER barriers[2]{};
	
	barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Transition.pResource = gbufferAlbedo.Get();
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[1].Transition.pResource = gbufferNormal.Get();
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(2, barriers);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};
	rtvHandles[0] = gbufferRtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandles[1].ptr = rtvHandles[0].ptr + gbufferRtvDescriptorSize;
	
	const D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };

	commandList->OMSetRenderTargets(2, rtvHandles, FALSE, &dsvHandle);

	const float clearAlbedo[4]{ 0.0f, 0.0f, 0.0f, 1.0f };
	const float clearNormal[4]{ 0.5f, 0.5f, 1.0f, 0.0f };
	commandList->ClearRenderTargetView(rtvHandles[0], clearAlbedo, 0, nullptr);
	commandList->ClearRenderTargetView(rtvHandles[1], clearNormal, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->SetGraphicsRootSignature(rootSignature.Get());

	return true;
}

void RenderSystem::EndFrame()
{
	if (commandList == nullptr || renderTargets[frameIndex] == nullptr)
	{
		return;
	}

	// Final transition of the Back Buffer to PRESENT state
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = renderTargets[frameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	if (FAILED(commandList->Close()))
	{
		return;
	}

	ID3D12CommandList* const ppCommandLists[]{ commandList.Get() };
	commandQueue->ExecuteCommandLists(1, ppCommandLists);
}

void RenderSystem::Present()
{
	if (swapChain == nullptr)
	{
		return;
	}
	if (FAILED(swapChain->Present(1, 0)))
	{
		return;
	}
	MoveToNextFrame();
}

void RenderSystem::SetCamera(Camera* camera_)
{
	camera = camera_;
	if (camera != nullptr && commandList != nullptr)
	{
		const Vector4D& rect{ camera->GetViewport() };
		D3D12_VIEWPORT vp{};
		vp.TopLeftX = rect.x * static_cast<float>(width);
		vp.TopLeftY = rect.y * static_cast<float>(height);
		vp.Width = rect.z * static_cast<float>(width);
		vp.Height = rect.w * static_cast<float>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		D3D12_RECT scissor{};
		scissor.left = static_cast<LONG>(vp.TopLeftX);
		scissor.top = static_cast<LONG>(vp.TopLeftY);
		scissor.right = static_cast<LONG>(vp.TopLeftX + vp.Width);
		scissor.bottom = static_cast<LONG>(vp.TopLeftY + vp.Height);

		commandList->RSSetViewports(1, &vp);
		commandList->RSSetScissorRects(1, &scissor);

		camera->UpdateFrustum();
		CameraConstants data{};
		data.viewMatrix = camera->GetViewMatrix();
		data.projectionMatrix = camera->GetProjectionMatrix();
		
		cameraCbvAddress = UploadConstantsData(data);
		commandList->SetGraphicsRootConstantBufferView(0, cameraCbvAddress);

		D3D12_CPU_DESCRIPTOR_HANDLE gbufferRtvs[2]{};
		gbufferRtvs[0] = gbufferRtvHeap->GetCPUDescriptorHandleForHeapStart();
		gbufferRtvs[1] = gbufferRtvs[0];
		gbufferRtvs[1].ptr += gbufferRtvDescriptorSize;

		switch (camera->GetClearMode())
		{
		case Camera::ClearType::SolidColor:
		{
			const ColorRGBA& clearColor{ camera->GetClearColor() };
			const float clearAlbedo[4]{ clearColor.x, clearColor.y, clearColor.z, clearColor.w };
			commandList->ClearRenderTargetView(gbufferRtvs[0], clearAlbedo, 0, nullptr);
			break;
		}
		case Camera::ClearType::DepthOnly:
		{
			break;
		}
		case Camera::ClearType::Nothing:
		{
			break;
		}
		}
	}
}

void RenderSystem::SetLights(std::span<Light* const> lights_)
{
	LightConstants data{};
	data.ambientColor = Vector4D{ 0.1f, 0.1f, 0.1f, 1.0f };

	if (camera != nullptr)
	{
		if (Transform* cameraTransform{ camera->GetOwner()->GetComponent<Transform>() })
		{
			data.cameraPosition = cameraTransform->GetWorldPosition();
		}
	}

	uint32_t activeCount{ 0 };
	for (Light* const light : lights_)
	{
		if (light == nullptr || light->GetOwner() == nullptr || !light->GetOwner()->IsActive())
		{
			continue;
		}
		if (activeCount >= 8)
		{
			break;
		}

		const ColorRGBA& lightColor{ light->GetColor() };
		data.lights[activeCount] = Vector4D{
			lightColor.x,
			lightColor.y,
			lightColor.z,
			light->GetIntensity()
		};

		if (Transform* lightTransform{ light->GetOwner()->GetComponent<Transform>() })
		{
			const Vector3D forward{ lightTransform->GetWorldMatrix().GetForward() };
			data.lightDirs[activeCount] = Vector4D{ forward.x, forward.y, forward.z, 1.0f };
		}

		++activeCount;
	}

	data.activeLightCount = activeCount;
	lightCbvAddress = UploadConstantsData(data);
}

void RenderSystem::ExecuteLightingPass()
{
	if (commandList == nullptr || renderTargets[frameIndex] == nullptr || srvHeap == nullptr || gbufferAlbedo == nullptr || gbufferNormal == nullptr)
	{
		return;
	}

	D3D12_RESOURCE_BARRIER gbufferBarriers[2]{};
	gbufferBarriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	gbufferBarriers[0].Transition.pResource = gbufferAlbedo.Get();
	gbufferBarriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	gbufferBarriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	gbufferBarriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	gbufferBarriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	gbufferBarriers[1].Transition.pResource = gbufferNormal.Get();
	gbufferBarriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	gbufferBarriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	gbufferBarriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(2, gbufferBarriers);

	D3D12_RESOURCE_BARRIER backBufferBarrier{};
	backBufferBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	backBufferBarrier.Transition.pResource = renderTargets[frameIndex].Get();
	backBufferBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	backBufferBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	backBufferBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &backBufferBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	rtvHandle.ptr += static_cast<size_t>(frameIndex) * rtvDescriptorSize;
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	float clearColor[4]{ 0.1f, 0.1f, 0.1f, 1.0f };
	if (camera != nullptr && camera->GetClearMode() == Camera::ClearType::SolidColor)
	{
		const ColorRGBA& cameraClear{ camera->GetClearColor() };
		clearColor[0] = cameraClear.x;
		clearColor[1] = cameraClear.y;
		clearColor[2] = cameraClear.z;
		clearColor[3] = cameraClear.w;
	}
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	commandList->SetPipelineState(lightingPipelineState.Get());
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetGraphicsRootConstantBufferView(0, cameraCbvAddress);
	commandList->SetGraphicsRootConstantBufferView(1, lightCbvAddress);

	ID3D12DescriptorHeap* const ppHeaps[]{ srvHeap.Get() };
	commandList->SetDescriptorHeaps(1, ppHeaps);
	commandList->SetGraphicsRootDescriptorTable(4, srvHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawInstanced(3, 1, 0, 0);
}

void RenderSystem::SetRenderTargetToBackBuffer()
{
	if (commandList == nullptr || renderTargets[frameIndex] == nullptr)
	{
		return;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	rtvHandle.ptr += static_cast<size_t>(frameIndex) * rtvDescriptorSize;
	
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	if (camera != nullptr)
	{
		const Vector4D& rect{ camera->GetViewport() };
		D3D12_VIEWPORT vp{};
		vp.TopLeftX = rect.x * static_cast<float>(width);
		vp.TopLeftY = rect.y * static_cast<float>(height);
		vp.Width = rect.z * static_cast<float>(width);
		vp.Height = rect.w * static_cast<float>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		D3D12_RECT scissor{};
		scissor.left = static_cast<LONG>(vp.TopLeftX);
		scissor.top = static_cast<LONG>(vp.TopLeftY);
		scissor.right = static_cast<LONG>(vp.TopLeftX + vp.Width);
		scissor.bottom = static_cast<LONG>(vp.TopLeftY + vp.Height);

		commandList->RSSetViewports(1, &vp);
		commandList->RSSetScissorRects(1, &scissor);
	}
}

ID3D12Device* RenderSystem::GetDevice() const noexcept { return device.Get(); }
ID3D12GraphicsCommandList* RenderSystem::GetCommandList() const noexcept { return commandList.Get(); }
const D3D12_VIEWPORT& RenderSystem::GetViewport() const noexcept { return viewport; }
ID3D12RootSignature* RenderSystem::GetDefaultRootSignature() const noexcept { return rootSignature.Get(); }
ID3D12PipelineState* RenderSystem::GetDefaultPipelineState() const noexcept { return pipelineState.Get(); }
ID3D12PipelineState* RenderSystem::GetUIPipelineState() const noexcept { return uiPipelineState.Get(); }
ID3D12PipelineState* RenderSystem::GetLightingPipelineState() const noexcept { return lightingPipelineState.Get(); }

void RenderSystem::SetPipelineState(ID3D12PipelineState* pipelineState_)
{
	if (commandList != nullptr && pipelineState_ != nullptr)
	{
		commandList->SetPipelineState(pipelineState_);
	}
}

void RenderSystem::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& indexBufferView_)
{
	if (commandList != nullptr)
	{
		commandList->IASetIndexBuffer(&indexBufferView_);
	}
}

void RenderSystem::SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView_)
{
	if (commandList != nullptr)
	{
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	}
}

void RenderSystem::DrawIndexedInstanced(UINT indexCountPerInstance_, UINT instanceCount_, UINT startIndexLocation_, INT baseVertexLocation_, UINT startInstanceLocation_)
{
	if (commandList != nullptr)
	{
		commandList->DrawIndexedInstanced(indexCountPerInstance_, instanceCount_, startIndexLocation_, baseVertexLocation_, startInstanceLocation_);
	}
}

void RenderSystem::DrawInstanced(UINT vertexCountPerInstance_, UINT instanceCount_, UINT startVertexLocation_, UINT startInstanceLocation_)
{
	if (commandList != nullptr)
	{
		commandList->DrawInstanced(vertexCountPerInstance_, instanceCount_, startVertexLocation_, startInstanceLocation_);
	}
}

void RenderSystem::SetObjectConstants(const ObjectConstants& data_)
{
	const D3D12_GPU_VIRTUAL_ADDRESS cbv{ UploadConstantsData(data_) };
	if (commandList != nullptr)
	{
		commandList->SetGraphicsRootConstantBufferView(2, cbv);
	}
}

void RenderSystem::SetMaterialConstants(const MaterialConstants& data_)
{
	const D3D12_GPU_VIRTUAL_ADDRESS cbv{ UploadConstantsData(data_) };
	if (commandList != nullptr)
	{
		commandList->SetGraphicsRootConstantBufferView(3, cbv);
	}
}

void RenderSystem::DrawMeshInstanced(Mesh* mesh_, Material* material_, std::span<const Matrix4x4> worldMatrices_)
{
	if (mesh_ == nullptr || material_ == nullptr || worldMatrices_.empty() || commandList == nullptr || device == nullptr || mappedInstanceData == nullptr)
	{
		return;
	}

	if (!mesh_->HasGpuBuffers() && !mesh_->CreateBuffers(device.Get()))
	{
		return;
	}

	MaterialConstants materialData{};
	materialData.color = material_->GetColor();
	SetMaterialConstants(materialData);

	ObjectConstants objectData{};
	objectData.worldMatrix = Matrix4x4::GetIdentity();
	SetObjectConstants(objectData);

	const uint32_t matrixSize{ static_cast<uint32_t>(sizeof(Matrix4x4)) };
	const uint32_t frameStartOffset{ frameIndex * (instanceBufferCapacity / SwapChainBufferCount) };
	const uint32_t frameEndOffset{ (frameIndex + 1) * (instanceBufferCapacity / SwapChainBufferCount) };
	const uint32_t frameCapacity{ frameEndOffset - frameStartOffset };
	const uint32_t maxInstancesPerChunk{ frameCapacity / matrixSize };
	if (maxInstancesPerChunk == 0)
	{
		return;
	}

	if (instanceBufferOffset < frameStartOffset || instanceBufferOffset >= frameEndOffset)
	{
		instanceBufferOffset = frameStartOffset;
	}

	uint32_t remaining{ static_cast<uint32_t>(worldMatrices_.size()) };
	uint32_t processed{ 0 };

	while (remaining > 0)
	{
		const uint32_t availableBytes{ frameEndOffset - instanceBufferOffset };
		uint32_t chunkCapacity{ availableBytes / matrixSize };
		if (chunkCapacity == 0)
		{
			return;
		}

		const uint32_t chunkCount{ std::min(remaining, chunkCapacity) };
		const uint32_t chunkBytes{ chunkCount * matrixSize };

		std::memcpy(mappedInstanceData + instanceBufferOffset, worldMatrices_.data() + processed, chunkBytes);

		D3D12_VERTEX_BUFFER_VIEW views[2]{};
		views[0] = mesh_->GetVertexBufferView();
		views[1].BufferLocation = instanceUploadBuffer->GetGPUVirtualAddress() + instanceBufferOffset;
		views[1].StrideInBytes = matrixSize;
		views[1].SizeInBytes = chunkBytes;

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 2, views);

		if (mesh_->GetIndexCount() > 0)
		{
			commandList->IASetIndexBuffer(&mesh_->GetIndexBufferView());
			commandList->DrawIndexedInstanced(mesh_->GetIndexCount(), chunkCount, 0, 0, 0);
		}
		else
		{
			commandList->DrawInstanced(static_cast<UINT>(mesh_->GetVertices().size()), chunkCount, 0, 0);
		}

		instanceBufferOffset += chunkBytes;
		processed += chunkCount;
		remaining -= chunkCount;
	}
}

std::expected<void, std::wstring> RenderSystem::CreateDevice()
{
	UINT factoryFlags{ 0 };
#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory))))
	{
		return std::unexpected<std::wstring>(L"Failed to create DXGI Factory.");
	}

	if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
	{
		return std::unexpected<std::wstring>(L"Failed to create D3D12 Device.");
	}

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
	{
		return std::unexpected<std::wstring>(L"Failed to create Command Queue.");
	}

	for (uint32_t i{ 0 }; i < SwapChainBufferCount; ++i)
	{
		if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]))))
		{
			return std::unexpected<std::wstring>(L"Failed to create Command Allocator.");
		}
	}

	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&commandList))))
	{
		return std::unexpected<std::wstring>(L"Failed to create Command List.");
	}

	commandList->Close();
	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateSwapChain(HWND hWnd_)
{
	RECT rect{};
	GetClientRect(hWnd_, &rect);
	width = static_cast<uint32_t>(rect.right - rect.left);
	height = static_cast<uint32_t>(rect.bottom - rect.top);

	viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	scissorRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

	DXGI_SWAP_CHAIN_DESC1 sd{};
	sd.Width = width;
	sd.Height = height;
	sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory))))
	{
		return std::unexpected<std::wstring>(L"Failed to create DXGI Factory.");
	}

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	if (FAILED(factory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd_, &sd, nullptr, nullptr, &swapChain1)))
	{
		return std::unexpected<std::wstring>(L"Failed to create Swap Chain.");
	}

	if (FAILED(swapChain1.As(&swapChain)))
	{
		return std::unexpected<std::wstring>(L"Failed to cast Swap Chain.");
	}
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap))))
	{
		return std::unexpected<std::wstring>(L"Failed to create RTV Heap.");
	}
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (uint32_t i{ 0 }; i < SwapChainBufferCount; ++i)
	{
		if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]))))
		{
			return std::unexpected<std::wstring>(L"Failed to get buffer.");
		}
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += rtvDescriptorSize;
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	if (FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap))))
	{
		return std::unexpected<std::wstring>(L"Failed to create DSV Heap.");
	}
	dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	D3D12_RESOURCE_DESC dsDesc{};
	dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsDesc.Width = width;
	dsDesc.Height = height;
	dsDesc.DepthOrArraySize = 1;
	dsDesc.MipLevels = 1;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES depthHeapProps{};
	depthHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE dsClear{};
	dsClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsClear.DepthStencil.Depth = 1.0f;

	if (FAILED(device->CreateCommittedResource(&depthHeapProps, D3D12_HEAP_FLAG_NONE, &dsDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &dsClear, IID_PPV_ARGS(&depthStencilBuffer))))
	{
		return std::unexpected<std::wstring>(L"Failed to create Depth Buffer.");
	}
	device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateGBuffers()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc{};
	rtvDesc.NumDescriptors = 2;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	if (FAILED(device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&gbufferRtvHeap))))
	{
		return std::unexpected<std::wstring>(L"Failed to create GBuffer RTV Heap.");
	}
	gbufferRtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC srvDesc{};
	srvDesc.NumDescriptors = 2;
	srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (FAILED(device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&srvHeap))))
	{
		return std::unexpected<std::wstring>(L"Failed to create SRV Heap.");
	}
	srvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_RESOURCE_DESC texDesc{};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clear{};
	clear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	clear.Color[0] = 0.0f;
	clear.Color[1] = 0.0f;
	clear.Color[2] = 0.0f;
	clear.Color[3] = 1.0f;

	if (FAILED(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clear, IID_PPV_ARGS(&gbufferAlbedo))))
	{
		return std::unexpected<std::wstring>(L"Failed to create Albedo GBuffer.");
	}
	if (FAILED(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clear, IID_PPV_ARGS(&gbufferNormal))))
	{
		return std::unexpected<std::wstring>(L"Failed to create Normal GBuffer.");
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ gbufferRtvHeap->GetCPUDescriptorHandleForHeapStart() };
	device->CreateRenderTargetView(gbufferAlbedo.Get(), nullptr, rtvHandle);
	rtvHandle.ptr += gbufferRtvDescriptorSize;
	device->CreateRenderTargetView(gbufferNormal.Get(), nullptr, rtvHandle);

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle{ srvHeap->GetCPUDescriptorHandleForHeapStart() };
	D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(gbufferAlbedo.Get(), &srv, srvHandle);
	srvHandle.ptr += srvDescriptorSize;
	device->CreateShaderResourceView(gbufferNormal.Get(), &srv, srvHandle);

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateRootSignature()
{
	D3D12_ROOT_PARAMETER params[5]{};
	params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[0].Descriptor.ShaderRegister = 0;
	params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[1].Descriptor.ShaderRegister = 1;
	params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[2].Descriptor.ShaderRegister = 2;
	params[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[3].Descriptor.ShaderRegister = 3;

	D3D12_DESCRIPTOR_RANGE range{};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range.NumDescriptors = 2;
	range.BaseShaderRegister = 0;
	range.OffsetInDescriptorsFromTableStart = 0;
	
	params[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[4].DescriptorTable.NumDescriptorRanges = 1;
	params[4].DescriptorTable.pDescriptorRanges = &range;

	D3D12_STATIC_SAMPLER_DESC sampler{};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	sampler.ShaderRegister = 0;

	D3D12_ROOT_SIGNATURE_DESC rd{};
	rd.NumParameters = 5;
	rd.pParameters = params;
	rd.NumStaticSamplers = 1;
	rd.pStaticSamplers = &sampler;
	rd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	if (FAILED(D3D12SerializeRootSignature(&rd, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
	{
		return std::unexpected<std::wstring>(L"Failed to serialize Root Signature.");
	}
	if (FAILED(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature))))
	{
		return std::unexpected<std::wstring>(L"Failed to create Root Signature.");
	}

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreatePipelineStates()
{
	Shader* const shader{ ResourceSystem::GetInstance().GetResource<Shader>(L"Resources/Shaders/GameObject.hlsl") };
	if (shader == nullptr)
	{
		return std::unexpected<std::wstring>(L"Failed to load GameObject shader.");
	}

	D3D12_INPUT_ELEMENT_DESC instancedIl[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "INSTANCE_WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
	};

	D3D12_INPUT_ELEMENT_DESC uiIl[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pd{};
	pd.InputLayout = { instancedIl, 6 };
	pd.pRootSignature = rootSignature.Get();
	pd.VS = { shader->GetVSBlob()->GetBufferPointer(), shader->GetVSBlob()->GetBufferSize() };
	pd.PS = { shader->GetPSBlob()->GetBufferPointer(), shader->GetPSBlob()->GetBufferSize() };
	pd.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pd.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pd.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	pd.DepthStencilState.DepthEnable = TRUE;
	pd.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pd.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pd.SampleMask = UINT_MAX;
	pd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pd.NumRenderTargets = 2;
	pd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pd.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pd.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pd.SampleDesc.Count = 1;

	if (FAILED(device->CreateGraphicsPipelineState(&pd, IID_PPV_ARGS(&pipelineState))))
	{
		return std::unexpected<std::wstring>(L"Failed to create pipelineState.");
	}

	Shader* const lShader{ ResourceSystem::GetInstance().GetResource<Shader>(L"Resources/Shaders/DeferredLighting.hlsl") };
	if (lShader == nullptr)
	{
		return std::unexpected<std::wstring>(L"Failed to load DeferredLighting shader.");
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC ld{ pd };
	ld.InputLayout = { nullptr, 0 };
	ld.VS = { lShader->GetVSBlob()->GetBufferPointer(), lShader->GetVSBlob()->GetBufferSize() };
	ld.PS = { lShader->GetPSBlob()->GetBufferPointer(), lShader->GetPSBlob()->GetBufferSize() };
	ld.NumRenderTargets = 1;
	ld.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	ld.RTVFormats[1] = DXGI_FORMAT_UNKNOWN;
	ld.DepthStencilState.DepthEnable = FALSE;
	if (FAILED(device->CreateGraphicsPipelineState(&ld, IID_PPV_ARGS(&lightingPipelineState))))
	{
		return std::unexpected<std::wstring>(L"Failed to create lightingPipelineState.");
	}

	Shader* const uShader{ ResourceSystem::GetInstance().GetResource<Shader>(L"Resources/Shaders/UIObject.hlsl") };
	if (uShader != nullptr)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC ud{ ld };
		ud.InputLayout = { uiIl, 2 };
		ud.VS = { uShader->GetVSBlob()->GetBufferPointer(), uShader->GetVSBlob()->GetBufferSize() };
		ud.PS = { uShader->GetPSBlob()->GetBufferPointer(), uShader->GetPSBlob()->GetBufferSize() };
		device->CreateGraphicsPipelineState(&ud, IID_PPV_ARGS(&uiPipelineState));
	}

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateConstantBuffer()
{
	D3D12_HEAP_PROPERTIES hp{};
	hp.Type = D3D12_HEAP_TYPE_UPLOAD;
	D3D12_RESOURCE_DESC rd{};
	rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	rd.Width = MaxConstantBufferSize;
	rd.Height = 1;
	rd.DepthOrArraySize = 1;
	rd.MipLevels = 1;
	rd.Format = DXGI_FORMAT_UNKNOWN;
	rd.SampleDesc.Count = 1;
	rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	
	if (FAILED(device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constantBuffer))))
	{
		return std::unexpected<std::wstring>(L"Failed to create Constant Buffer.");
	}

	if (FAILED(constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedConstantData))))
	{
		return std::unexpected<std::wstring>(L"Failed to map Constant Buffer.");
	}

	D3D12_RESOURCE_DESC instanceDesc{};
	instanceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	instanceDesc.Width = instanceBufferCapacity;
	instanceDesc.Height = 1;
	instanceDesc.DepthOrArraySize = 1;
	instanceDesc.MipLevels = 1;
	instanceDesc.Format = DXGI_FORMAT_UNKNOWN;
	instanceDesc.SampleDesc.Count = 1;
	instanceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	if (FAILED(device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &instanceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&instanceUploadBuffer))))
	{
		return std::unexpected<std::wstring>(L"Failed to create Instance Upload Buffer.");
	}

	if (FAILED(instanceUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedInstanceData))))
	{
		return std::unexpected<std::wstring>(L"Failed to map Instance Upload Buffer.");
	}

	return {};
}

std::expected<void, std::wstring> RenderSystem::CreateSyncObjects()
{
	if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
	{
		return std::unexpected<std::wstring>(L"Failed to create Fence.");
	}
	fenceValues[0] = 0;
	fenceValues[1] = 0;
	fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		return std::unexpected<std::wstring>(L"Failed to create Fence Event.");
	}
	return {};
}

void RenderSystem::MoveToNextFrame()
{
	if (commandQueue == nullptr || fence == nullptr) return;
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

void RenderSystem::WaitForGpu()
{
	if (commandQueue == nullptr || fence == nullptr || fenceEvent == nullptr) return;
	const uint64_t current{ fenceValues[frameIndex] };
	commandQueue->Signal(fence.Get(), current);
	fence->SetEventOnCompletion(current, fenceEvent);
	WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	fenceValues[frameIndex]++;
}
