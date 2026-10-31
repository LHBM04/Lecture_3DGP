#pragma once

#include <d3d12.h>

namespace TUK::Framework
{
	class GPUTexture;

	struct GPUTargetProperties final
	{
		const GPUTexture* texture;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		DXGI_FORMAT format;
	};

	class GPUTarget
	{
	public:
		explicit GPUTarget(const GPUTargetProperties& properties) noexcept;
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

	inline GPUTarget::GPUTarget(const GPUTargetProperties& properties) noexcept
		: texture(properties.texture)
		, rtvHandle(properties.rtvHandle)
		, format(properties.format)
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
