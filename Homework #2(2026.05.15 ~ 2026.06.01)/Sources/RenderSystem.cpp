#include "Precompiled.h"
#include "RenderSystem.h"

RenderSystem::~RenderSystem() noexcept
{
	Release();
}

bool RenderSystem::Initialize(const RendererOptions& options_)
{
	(void)options_;
	return true;
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
	if (!renderer->Initialize(options_))
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
