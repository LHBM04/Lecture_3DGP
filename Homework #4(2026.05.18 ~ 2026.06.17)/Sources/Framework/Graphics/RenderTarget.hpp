#pragma once

#include <optional>
#include "Texture.hpp"

namespace TUK::Framework
{
	class RenderTarget final
	{
		friend class Renderer;
		friend class RenderSubsystem;

	public:
		RenderTarget() noexcept = default;
		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;
		RenderTarget(RenderTarget&&) noexcept = default;
		RenderTarget& operator=(RenderTarget&&) noexcept = default;

		[[nodiscard]] UINT GetWidth() const noexcept { return color.GetWidth(); }
		[[nodiscard]] UINT GetHeight() const noexcept { return color.GetHeight(); }
		[[nodiscard]] Texture& GetTexture() noexcept { return color; }
		[[nodiscard]] bool IsWindowTarget() const noexcept { return isWindowTarget; }

	private:
		Texture color{};
		std::optional<Texture> depth;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderViewHeap{};
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> depthViewHeap{};
		bool isWindowTarget = false;
	};
}
