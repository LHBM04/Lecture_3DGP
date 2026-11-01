#pragma once

#include <wrl.h>

#include <d3d12.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUTexture
	{
	public:
		explicit GPUTexture(ComPtr<ID3D12Resource> d3dResource) noexcept;
		~GPUTexture() noexcept = default;

		GPUTexture(const GPUTexture&) = delete;
		GPUTexture& operator=(const GPUTexture&) = delete;

		GPUTexture(GPUTexture&&) noexcept = default;
		GPUTexture& operator=(GPUTexture&&) noexcept = default;

		[[nodiscard]] ID3D12Resource& GetD3DResource() const noexcept;

	private:
		ComPtr<ID3D12Resource> d3dResource;
	};

	inline GPUTexture::GPUTexture(ComPtr<ID3D12Resource> d3dResource) noexcept
		: d3dResource(std::move(d3dResource))
	{
	}

	inline ID3D12Resource& GPUTexture::GetD3DResource() const noexcept
	{
		return *d3dResource.Get();
	}
}
