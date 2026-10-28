#pragma once

#include <cassert>

#include <d3d12.h>

#include "GPUTexture.hpp"

namespace TUK::Framework
{
	class GPUTarget
	{
		friend class GPUSwapChain;

	public:
		~GPUTarget() noexcept = default;

		GPUTarget(const GPUTarget&) = delete;
		GPUTarget& operator=(const GPUTarget&) = delete;

		GPUTarget(GPUTarget&&) noexcept = default;
		GPUTarget& operator=(GPUTarget&&) noexcept = default;

		[[nodiscard]] const GPUTexture& GetTexture() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3DRTVHandle() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetDXGIFormat() const noexcept;

	private:
		explicit GPUTarget(
			const GPUTexture& texture,
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
			DXGI_FORMAT format) noexcept;

		const GPUTexture* texture;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		DXGI_FORMAT format;
	};

	inline const GPUTexture& GPUTarget::GetTexture() const noexcept
	{
		assert(texture);
		return *texture;
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE GPUTarget::GetD3DRTVHandle() const noexcept
	{
		return rtvHandle;
	}

	inline DXGI_FORMAT GPUTarget::GetDXGIFormat() const noexcept
	{
		return format;
	}

	inline GPUTarget::GPUTarget(
		const GPUTexture& texture,
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
		DXGI_FORMAT format) noexcept
		: texture(&texture)
		, rtvHandle(rtvHandle)
		, format(format)
	{
	}
}
