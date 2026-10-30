#pragma once

#include <utility>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	class D3DTexture
	{
		friend class RenderSubsystem;

	public:
		~D3DTexture() noexcept = default;

		D3DTexture(const D3DTexture&) = delete;
		D3DTexture& operator=(const D3DTexture&) = delete;

		D3DTexture(D3DTexture&&) noexcept = default;
		D3DTexture& operator=(D3DTexture&&) noexcept = default;

		[[nodiscard]] ID3D12Resource& GetD3DResource() const noexcept;

	private:
		explicit D3DTexture(Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource) noexcept;

		Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource;
	};

	inline ID3D12Resource& D3DTexture::GetD3DResource() const noexcept
	{
		return *d3dResource.Get();
	}

	inline D3DTexture::D3DTexture(Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource) noexcept
		: d3dResource(std::move(d3dResource))
	{
	}
}
