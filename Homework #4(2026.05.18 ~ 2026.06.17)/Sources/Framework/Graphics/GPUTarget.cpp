#include "Precompiled.hpp"
#include "GPUTarget.hpp"

namespace TUK::Framework
{
	GPUTarget::GPUTarget(
		Microsoft::WRL::ComPtr<ID3D12Resource2> d3dResource,
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle) noexcept
		: d3dResource(std::move(d3dResource))
		, rtvHandle(rtvHandle)
	{
	}

	ID3D12Resource& GPUTarget::GetD3DResource() const noexcept
	{
		return *d3dResource.Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GPUTarget::GetRTVHandle() const noexcept
	{
		return rtvHandle;
	}
}
