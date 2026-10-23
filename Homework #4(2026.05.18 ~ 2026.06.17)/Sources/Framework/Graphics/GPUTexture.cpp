#include "Precompiled.hpp"
#include "GPUTexture.hpp"

namespace TUK::Framework
{
	ID3D12Resource& GPUTexture::GetD3DResource() const noexcept
	{
		assert(d3dResource);
		return *d3dResource.Get();
	}

	GPUTexture::GPUTexture(Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource) noexcept
		: d3dResource(std::move(d3dResource))
	{
	}
}
