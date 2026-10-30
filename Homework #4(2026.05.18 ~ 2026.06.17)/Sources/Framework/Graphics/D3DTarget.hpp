#pragma once

#include <cassert>

#include <d3d12.h>

#include "D3DTexture.hpp"

namespace TUK::Framework
{
	class D3DTarget
	{
		friend class D3DSwapChain;

	public:
		~D3DTarget() noexcept = default;

		D3DTarget(const D3DTarget&) = delete;
		D3DTarget& operator=(const D3DTarget&) = delete;

		D3DTarget(D3DTarget&&) noexcept = default;
		D3DTarget& operator=(D3DTarget&&) noexcept = default;

		[[nodiscard]] const D3DTexture& GetTexture() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3DRTVHandle() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetDXGIFormat() const noexcept;

	private:
		explicit D3DTarget(
			const D3DTexture& texture,
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
			DXGI_FORMAT format) noexcept;

		const D3DTexture* texture;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		DXGI_FORMAT format;
	};

	inline const D3DTexture& D3DTarget::GetTexture() const noexcept
	{
		assert(texture);
		return *texture;
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE D3DTarget::GetD3DRTVHandle() const noexcept
	{
		return rtvHandle;
	}

	inline DXGI_FORMAT D3DTarget::GetDXGIFormat() const noexcept
	{
		return format;
	}

	inline D3DTarget::D3DTarget(
		const D3DTexture& texture,
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
		DXGI_FORMAT format) noexcept
		: texture(&texture)
		, rtvHandle(rtvHandle)
		, format(format)
	{
	}
}
