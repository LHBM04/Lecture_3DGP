#pragma once

#include "Renderer.h"
#include "System.h"
#include "Window.h"

struct RenderTargetHandle final
{
	uint32_t id{ UINT32_MAX };
	uint32_t generation{ 0 };

	[[nodiscard]] bool IsValid() const noexcept
	{
		return UINT32_MAX != id;
	}
};

class RenderSystem final : public System<RendererOptions>
{
public:
	~RenderSystem() noexcept override;

	bool Initialize(const RendererOptions& options_) override;
	void Release() override;

	// SDL-like: Window로부터 Renderer를 생성한다.
	[[nodiscard]] RenderTargetHandle CreateRenderer(Window& window_, const RendererOptions& options_);
	void DestroyRenderer(RenderTargetHandle handle_) noexcept;

	[[nodiscard]] Renderer* GetRenderTarget(RenderTargetHandle handle_) noexcept;
	[[nodiscard]] const Renderer* GetRenderTarget(RenderTargetHandle handle_) const noexcept;
	[[nodiscard]] RenderTargetHandle FindRenderTarget(HWND windowHandle_) const noexcept;

	void OnWindowResize(HWND windowHandle_, int width_, int height_);
	void OnWindowFullscreenToggle(HWND windowHandle_);

private:
	struct RenderTargetSlot final
	{
		HWND windowHandle{ nullptr };
		uint32_t generation{ 1 };
		std::unique_ptr<Renderer> renderer;
	};

	[[nodiscard]] RenderTargetSlot* TryGetSlot(RenderTargetHandle handle_) noexcept;
	[[nodiscard]] const RenderTargetSlot* TryGetSlot(RenderTargetHandle handle_) const noexcept;

	std::vector<RenderTargetSlot> renderTargets;
};
