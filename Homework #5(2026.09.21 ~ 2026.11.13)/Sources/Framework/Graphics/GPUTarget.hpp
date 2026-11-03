#pragma once

#include <directx/d3d12.h>

namespace TUK::Framework
{
	class GPUTexture;

	class GPUTarget
	{
	public:
		GPUTarget(const GPUTexture& texture, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, DXGI_FORMAT format) noexcept;
		~GPUTarget() noexcept = default;

		GPUTarget(const GPUTarget&) = delete;
		GPUTarget& operator=(const GPUTarget&) = delete;

		GPUTarget(GPUTarget&&) = delete;
		GPUTarget& operator=(GPUTarget&&) = delete;

		[[nodiscard]] const GPUTexture& GetTexture() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3DRTVHandle() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetDXGIFormat() const noexcept;

	private:
		const GPUTexture* texture;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		DXGI_FORMAT format;
	};

	inline GPUTarget::GPUTarget(const GPUTexture& texture, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, DXGI_FORMAT format) noexcept
		: texture(&texture)
		, rtvHandle(rtvHandle)
		, format(format)
	{
	}

	inline const GPUTexture& GPUTarget::GetTexture() const noexcept
	{
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
}
