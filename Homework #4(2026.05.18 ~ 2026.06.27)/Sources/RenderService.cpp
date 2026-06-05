#include "Precompiled.h"
#include "RenderService.h"

#include "DescriptorAllocator.h"
#include "Framework.h"
#include "RenderContext.h"
#include "RenderDevice.h"
#include "SwapChain.h"
#include "UploadBuffer.h"
#include "Window.h"
#include "WindowService.h"

RenderService::RenderService() noexcept = default;

RenderService::~RenderService() noexcept = default;

bool RenderService::IsInitialized() const noexcept
{
	return isInitialized;
}

void RenderService::BeginFrame()
{
	assert(device != nullptr);
	assert(context != nullptr);
	assert(swapChain != nullptr);
	assert(rtvAllocator != nullptr);
	assert(dsvAllocator != nullptr);
	assert(srvAllocator != nullptr);
	assert(uploadBuffers[frameIndex] != nullptr);

	uploadBuffers[frameIndex]->Reset();
	context->SetUploadBuffer(*uploadBuffers[frameIndex]);
	context->Reset(frameIndex);

	ID3D12GraphicsCommandList& commandList{ context->GetCommandList() };
	ID3D12Resource& backBuffer{ swapChain->GetCurrentBackBuffer(frameIndex) };

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = &backBuffer;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList.ResourceBarrier(1, &barrier);

	const D3D12_VIEWPORT& viewport{ swapChain->GetViewport() };
	const D3D12_RECT& scissorRect{ swapChain->GetScissorRect() };
	commandList.RSSetViewports(1, &viewport);
	commandList.RSSetScissorRects(1, &scissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ swapChain->GetCurrentRtvHandle(frameIndex) };
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ swapChain->GetDsvHandle() };
	context->SetRenderTargetHandles(rtvHandle, dsvHandle);
	commandList.OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void RenderService::Clear()
{
	assert(context != nullptr);
	assert(swapChain != nullptr);

	constexpr FLOAT clearColor[]{ 0.05f, 0.08f, 0.13f, 1.0f };
	ID3D12GraphicsCommandList& commandList{ context->GetCommandList() };
	commandList.ClearRenderTargetView(swapChain->GetCurrentRtvHandle(frameIndex), clearColor, 0, nullptr);
	commandList.ClearDepthStencilView(swapChain->GetDsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderService::Present()
{
	assert(context != nullptr);
	assert(swapChain != nullptr);

	ID3D12GraphicsCommandList& commandList{ context->GetCommandList() };
	ID3D12Resource& backBuffer{ swapChain->GetCurrentBackBuffer(frameIndex) };

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = &backBuffer;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	commandList.ResourceBarrier(1, &barrier);

	context->Execute();
	swapChain->GetDXGISwapChain().Present(1, 0);
	context->MoveToNextFrame(frameIndex, swapChain->GetDXGISwapChain());
}

void RenderService::OnAdd()
{
	Framework& framework{ GetFramework() };
	Window* mainWindow{ framework.GetService<WindowService>().GetMainWindow() };
	if (mainWindow == nullptr)
	{
		return;
	}

	device = std::make_unique<RenderDevice>();
	if (!device->Initialize())
	{
		device.reset();
		return;
	}

	{
		RenderContextOptions options{};
		options.device = &device->GetDevice();
		options.backBufferCount = BackBufferCount;

		context = std::make_unique<RenderContext>();
		if (!context->Initialize(options))
		{
			OnRemove();
			return;
		}
	}

	{
		DescriptorAllocatorOptions options{};
		options.device = &device->GetDevice();
		options.type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		options.flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		options.numDescriptors = BackBufferCount;

		rtvAllocator = std::make_unique<DescriptorAllocator>();
		if (!rtvAllocator->Initialize(options))
		{
			OnRemove();
			return;
		}
	}

	{
		DescriptorAllocatorOptions options{};
		options.device = &device->GetDevice();
		options.type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		options.flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		options.numDescriptors = 1;

		dsvAllocator = std::make_unique<DescriptorAllocator>();
		if (!dsvAllocator->Initialize(options))
		{
			OnRemove();
			return;
		}
	}

	{
		DescriptorAllocatorOptions options{};
		options.device = &device->GetDevice();
		options.type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		options.flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		options.numDescriptors = 1024;

		srvAllocator = std::make_unique<DescriptorAllocator>();
		if (!srvAllocator->Initialize(options))
		{
			OnRemove();
			return;
		}
	}

	{
		SwapChainOptions options{};
		options.factory = &device->GetFactory();
		options.device = &device->GetDevice();
		options.commandQueue = &context->GetCommandQueue();
		options.window = mainWindow->GetHWND();
		options.backBufferCount = BackBufferCount;
		options.rtvAllocator = rtvAllocator.get();
		options.dsvAllocator = dsvAllocator.get();

		swapChain = std::make_unique<SwapChain>();
		if (!swapChain->Initialize(options))
		{
			OnRemove();
			return;
		}
	}

	frameIndex = swapChain->GetDXGISwapChain().GetCurrentBackBufferIndex();

	for (std::unique_ptr<UploadBuffer>& uploadBuffer : uploadBuffers)
	{
		UploadBufferOptions options{};
		options.device = &device->GetDevice();
		options.bufferSize = FrameConstantBufferSize;

		uploadBuffer = std::make_unique<UploadBuffer>();
		if (!uploadBuffer->Initialize(options))
		{
			OnRemove();
			return;
		}
	}

	isInitialized = true;
}

void RenderService::OnRemove()
{
	isInitialized = false;

	if (context != nullptr && swapChain != nullptr)
	{
		context->WaitForGpu(frameIndex);
	}

	for (std::unique_ptr<UploadBuffer>& uploadBuffer : uploadBuffers)
	{
		if (uploadBuffer != nullptr)
		{
			uploadBuffer->Release();
			uploadBuffer.reset();
		}
	}

	if (swapChain != nullptr)
	{
		swapChain->Release();
		swapChain.reset();
	}

	if (dsvAllocator != nullptr)
	{
		dsvAllocator->Release();
		dsvAllocator.reset();
	}

	if (srvAllocator != nullptr)
	{
		srvAllocator->Release();
		srvAllocator.reset();
	}

	if (rtvAllocator != nullptr)
	{
		rtvAllocator->Release();
		rtvAllocator.reset();
	}

	if (context != nullptr)
	{
		context->Release();
		context.reset();
	}

	if (device != nullptr)
	{
		device->Release();
		device.reset();
	}

	frameIndex = 0;
}
