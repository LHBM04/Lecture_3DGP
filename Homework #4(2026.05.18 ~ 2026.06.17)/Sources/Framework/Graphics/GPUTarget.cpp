#include "Precompiled.hpp"
#include "GPUTarget.hpp"

namespace TUK::Framework
{
	const GPUTexture& GPUTarget::GetTexture() const noexcept
	{
		assert(texture);
		return *texture;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GPUTarget::GetD3DRTVHandle() const noexcept
	{
		return rtvHandle;
	}

	DXGI_FORMAT GPUTarget::GetDXGIFormat() const noexcept
	{
		return format;
	}

	GPUTarget::GPUTarget(
		const GPUTexture& texture,
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
		DXGI_FORMAT format) noexcept
		: texture(&texture)
		, rtvHandle(rtvHandle)
		, format(format)
	{
	}
}
