#pragma once

#include <cassert>

#include <d3d12.h>

#include "GPUTexture.hpp"

namespace TUK::Framework
{
	class GPURenderTarget
	{
		friend class GPUSwapChain;

	public:
		~GPURenderTarget() noexcept = default;

		GPURenderTarget(const GPURenderTarget&) = delete;
		GPURenderTarget& operator=(const GPURenderTarget&) = delete;

		GPURenderTarget(GPURenderTarget&&) noexcept = default;
		GPURenderTarget& operator=(GPURenderTarget&&) noexcept = default;

		[[nodiscard]] const GPUTexture& GetTexture() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3DRTVHandle() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetDXGIFormat() const noexcept;

	private:
		explicit GPURenderTarget(
			const GPUTexture& texture,
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
			DXGI_FORMAT format) noexcept;

		const GPUTexture* texture;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		DXGI_FORMAT format;
	};

	inline const GPUTexture& GPURenderTarget::GetTexture() const noexcept
	{
		assert(texture);
		return *texture;
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE GPURenderTarget::GetD3DRTVHandle() const noexcept
	{
		return rtvHandle;
	}

	inline DXGI_FORMAT GPURenderTarget::GetDXGIFormat() const noexcept
	{
		return format;
	}

	inline GPURenderTarget::GPURenderTarget(
		const GPUTexture& texture,
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
		DXGI_FORMAT format) noexcept
		: texture(&texture)
		, rtvHandle(rtvHandle)
		, format(format)
	{
	}
}
