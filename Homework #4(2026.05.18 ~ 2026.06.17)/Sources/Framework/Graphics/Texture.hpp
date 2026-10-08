#pragma once

#include <cassert>
#include <d3d12.h>
#include <wrl.h>

namespace TUK::Framework
{
	class Renderer;
	class RenderSubsystem;
	class RenderTarget;

	/** 이 리소스를 기록한 프레임이 끝날 때까지 이동하거나 해제하지 않는다. */
	class Texture final
	{
		friend class Renderer;
		friend class RenderSubsystem;
		friend class RenderTarget;

	public:
		Texture() noexcept = default;
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&&) noexcept = default;
		Texture& operator=(Texture&&) noexcept = default;

		[[nodiscard]] UINT GetWidth() const noexcept { return width; }
		[[nodiscard]] UINT GetHeight() const noexcept { return height; }
		[[nodiscard]] DXGI_FORMAT GetFormat() const noexcept { return format; }
		/** 업로드 및 추가 복사 명령용이다. 상태 전환은 Renderer::Transition으로 기록한다. */
		[[nodiscard]] ID3D12Resource& GetNativeResource() const noexcept { assert(resource); return *resource.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource{};
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> shaderViewHeap{};
		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		UINT width = 0;
		UINT height = 0;
	};
}
