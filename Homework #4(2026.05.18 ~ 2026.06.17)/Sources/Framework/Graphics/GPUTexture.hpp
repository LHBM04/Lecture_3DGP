#pragma once

#include <utility>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	class GPUTexture
	{
		friend class RenderSubsystem;

	public:
		~GPUTexture() noexcept = default;

		GPUTexture(const GPUTexture&) = delete;
		GPUTexture& operator=(const GPUTexture&) = delete;

		GPUTexture(GPUTexture&&) noexcept = default;
		GPUTexture& operator=(GPUTexture&&) noexcept = default;

		[[nodiscard]] ID3D12Resource& GetD3DResource() const noexcept;

	private:
		explicit GPUTexture(Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource) noexcept;

		Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource;
	};

	inline ID3D12Resource& GPUTexture::GetD3DResource() const noexcept
	{
		return *d3dResource.Get();
	}

	inline GPUTexture::GPUTexture(Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource) noexcept
		: d3dResource(std::move(d3dResource))
	{
	}
}
