#pragma once

#include <directx/d3d12.h>

namespace TUK::Framework
{
	class GPUTexture2D;

	class GPUTarget
	{
	public:
		static constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
		static constexpr float DEPTH_CLEAR_VALUE = 1.0f;

		GPUTarget(
			const GPUTexture2D& texture,
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
			DXGI_FORMAT format) noexcept;

		GPUTarget(const GPUTarget&) = delete;
		GPUTarget& operator=(const GPUTarget&) = delete;

		GPUTarget(GPUTarget&&) = delete;
		GPUTarget& operator=(GPUTarget&&) = delete;

		[[nodiscard]] const GPUTexture2D& GetTexture() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3DRTVHandle() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3DDSVHandle() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetDXGIFormat() const noexcept;

	private:
		const GPUTexture2D* texture;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
		DXGI_FORMAT format;
	};

	inline GPUTarget::GPUTarget(
		const GPUTexture2D& texture,
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
		DXGI_FORMAT format) noexcept
		: texture(&texture)
		, rtvHandle(rtvHandle)
		, dsvHandle(dsvHandle)
		, format(format)
	{
	}

	inline const GPUTexture2D& GPUTarget::GetTexture() const noexcept
	{
		return *texture;
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE GPUTarget::GetD3DRTVHandle() const noexcept
	{
		return rtvHandle;
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE GPUTarget::GetD3DDSVHandle() const noexcept
	{
		return dsvHandle;
	}

	inline DXGI_FORMAT GPUTarget::GetDXGIFormat() const noexcept
	{
		return format;
	}
}
