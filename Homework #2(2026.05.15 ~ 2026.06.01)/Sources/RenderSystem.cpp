#include "Precompiled.h"
#include "RenderSystem.h"

RenderSystem::~RenderSystem() noexcept
{
	Release();
}

bool RenderSystem::Initialize(const RendererOptions& options_)
{
	(void)options_;
	CreateDevice();
	CreateCommandQueue();
	CreateFence();

	return nullptr != factory &&
		nullptr != device &&
		nullptr != commandQueue &&
		nullptr != fence &&
		nullptr != fenceEvent;
}

void RenderSystem::Release()
{
	for (RenderTargetSlot& target : renderTargets)
	{
		if (target.renderer)
		{
			target.renderer->Release();
			target.renderer.reset();
		}
	}

	renderTargets.clear();

	fence.Reset();
	commandQueue.Reset();
	device.Reset();
	factory.Reset();

	if (nullptr != fenceEvent)
	{
		::CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

#if defined(_DEBUG)
	debugController.Reset();
#endif

	nextFenceValue = 1;
}

RenderTargetHandle RenderSystem::CreateRenderer(Window& window_, const RendererOptions& options_)
{
	const HWND windowHandle{ window_.GetHandle() };
	if (nullptr == windowHandle)
	{
		return {};
	}

	const RenderTargetHandle existingHandle{ FindRenderTarget(windowHandle) };
	if (existingHandle.IsValid())
	{
		return existingHandle;
	}

	std::unique_ptr<Renderer> renderer{ std::make_unique<Renderer>() };
	if (!renderer->Initialize(*this, options_))
	{
		return {};
	}

	RenderTargetSlot slot{};
	slot.windowHandle = windowHandle;
	slot.renderer = std::move(renderer);
	renderTargets.push_back(std::move(slot));

	return RenderTargetHandle{
		.id = static_cast<uint32_t>(renderTargets.size() - 1u),
		.generation = renderTargets.back().generation
	};
}

void RenderSystem::DestroyRenderer(RenderTargetHandle handle_) noexcept
{
	RenderTargetSlot* slot{ TryGetSlot(handle_) };
	if (nullptr == slot)
	{
		return;
	}

	if (slot->renderer)
	{
		slot->renderer->Release();
		slot->renderer.reset();
	}

	slot->windowHandle = nullptr;
	++slot->generation;
}

Renderer* RenderSystem::GetRenderTarget(RenderTargetHandle handle_) noexcept
{
	RenderTargetSlot* slot{ TryGetSlot(handle_) };
	return nullptr != slot ? slot->renderer.get() : nullptr;
}

const Renderer* RenderSystem::GetRenderTarget(RenderTargetHandle handle_) const noexcept
{
	const RenderTargetSlot* slot{ TryGetSlot(handle_) };
	return nullptr != slot ? slot->renderer.get() : nullptr;
}

ID3D12Device* RenderSystem::GetDevice() const noexcept
{
	return device.Get();
}

RenderTargetHandle RenderSystem::FindRenderTarget(HWND windowHandle_) const noexcept
{
	for (uint32_t i{ 0u }; i < static_cast<uint32_t>(renderTargets.size()); ++i)
	{
		const RenderTargetSlot& slot{ renderTargets[i] };
		if (slot.windowHandle == windowHandle_ && slot.renderer)
		{
			return RenderTargetHandle{ .id = i, .generation = slot.generation };
		}
	}

	return {};
}

void RenderSystem::OnWindowResize(HWND windowHandle_, int width_, int height_)
{
	const RenderTargetHandle handle{ FindRenderTarget(windowHandle_) };
	Renderer* renderer{ GetRenderTarget(handle) };
	if (nullptr == renderer)
	{
		return;
	}

	renderer->Resize(width_, height_);
}

void RenderSystem::OnWindowFullscreenToggle(HWND windowHandle_)
{
	const RenderTargetHandle handle{ FindRenderTarget(windowHandle_) };
	Renderer* renderer{ GetRenderTarget(handle) };
	if (nullptr == renderer)
	{
		return;
	}

	renderer->ToggleFullscreen();
}

RenderSystem::RenderTargetSlot* RenderSystem::TryGetSlot(RenderTargetHandle handle_) noexcept
{
	if (!handle_.IsValid() || handle_.id >= renderTargets.size())
	{
		return nullptr;
	}

	RenderTargetSlot& slot{ renderTargets[handle_.id] };
	if (slot.generation != handle_.generation || !slot.renderer)
	{
		return nullptr;
	}

	return &slot;
}

const RenderSystem::RenderTargetSlot* RenderSystem::TryGetSlot(RenderTargetHandle handle_) const noexcept
{
	if (!handle_.IsValid() || handle_.id >= renderTargets.size())
	{
		return nullptr;
	}

	const RenderTargetSlot& slot{ renderTargets[handle_.id] };
	if (slot.generation != handle_.generation || !slot.renderer)
	{
		return nullptr;
	}

	return &slot;
}

void RenderSystem::CreateDevice()
{
	UINT factoryFlags{ 0 };

#if defined(_DEBUG)
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
	{
		debugController->EnableDebugLayer();
		factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	assert(SUCCEEDED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory))));
	CreateWarpDevice();
}

void RenderSystem::CreateWarpDevice()
{
	assert(nullptr != factory);

	Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
	assert(SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))));
	assert(SUCCEEDED(D3D12CreateDevice(
		warpAdapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&device))));
}

void RenderSystem::CreateCommandQueue()
{
	assert(nullptr != device);

	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	assert(SUCCEEDED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue))));
}

void RenderSystem::CreateFence()
{
	assert(nullptr != device);
	assert(SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))));

	fenceEvent = ::CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	assert(nullptr != fenceEvent);
	nextFenceValue = 1;
}

void RenderSystem::WaitForFence(UINT64 fenceValue_)
{
	if (nullptr == fence || nullptr == fenceEvent || 0 == fenceValue_)
	{
		return;
	}

	if (fence->GetCompletedValue() >= fenceValue_)
	{
		return;
	}

	if (SUCCEEDED(fence->SetEventOnCompletion(fenceValue_, fenceEvent)))
	{
		::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}
}

UINT64 RenderSystem::SignalFence()
{
	const UINT64 fenceValue{ nextFenceValue++ };
	if (FAILED(commandQueue->Signal(fence.Get(), fenceValue)))
	{
		return 0;
	}

	return fenceValue;
}
