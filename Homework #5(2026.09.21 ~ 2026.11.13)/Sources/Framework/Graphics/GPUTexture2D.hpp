#pragma once

#include <utility>

#include <wrl/client.h>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	class GPUTexture2D
	{
	public:
		explicit GPUTexture2D(Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource) noexcept;

		GPUTexture2D(const GPUTexture2D&) = delete;
		GPUTexture2D& operator=(const GPUTexture2D&) = delete;

		GPUTexture2D(GPUTexture2D&&) noexcept = default;
		GPUTexture2D& operator=(GPUTexture2D&&) noexcept = default;

		[[nodiscard]] ID3D12Resource& GetD3DResource() const noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource;
	};

	inline GPUTexture2D::GPUTexture2D(Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource) noexcept
		: d3dResource(std::move(d3dResource))
	{
		ASSERT_FMT(this->d3dResource.Get(), "d3dResource가 nullptr입니다.");
	}

	inline ID3D12Resource& GPUTexture2D::GetD3DResource() const noexcept
	{
		ASSERT_FMT(d3dResource.Get(), "d3dResource가 nullptr입니다.");

		return *d3dResource.Get();
	}
}
