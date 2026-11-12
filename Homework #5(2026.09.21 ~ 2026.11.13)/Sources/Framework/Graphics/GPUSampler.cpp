#include "Precompiled.hpp"
#include "GPUSampler.hpp"

namespace TUK::Framework
{
	CD3DX12_STATIC_SAMPLER_DESC GPUSampler::ToStaticDescription(
		u32 shaderRegister,
		u32 registerSpace,
		D3D12_SHADER_VISIBILITY visibility) const noexcept
	{
		return CD3DX12_STATIC_SAMPLER_DESC(
			shaderRegister,
			description.Filter,
			description.AddressU,
			description.AddressV,
			description.AddressW,
			description.MipLODBias,
			description.MaxAnisotropy,
			description.ComparisonFunc,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
			description.MinLOD,
			description.MaxLOD,
			visibility,
			registerSpace);
	}

	GPUSampler GPUSampler::LinearWrap() noexcept
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.MaxLOD = D3D12_FLOAT32_MAX;
		return GPUSampler(desc);
	}

	GPUSampler GPUSampler::LinearClamp() noexcept
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.MaxLOD = D3D12_FLOAT32_MAX;
		return GPUSampler(desc);
	}

	GPUSampler GPUSampler::PointClamp() noexcept
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.MaxLOD = D3D12_FLOAT32_MAX;
		return GPUSampler(desc);
	}

	GPUSampler GPUSampler::PointWrap() noexcept
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.MaxLOD = D3D12_FLOAT32_MAX;
		return GPUSampler(desc);
	}

	GPUSampler GPUSampler::AnisotropicWrap(u32 maxAnisotropy) noexcept
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_ANISOTROPIC;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.MaxAnisotropy = maxAnisotropy;
		desc.MaxLOD = D3D12_FLOAT32_MAX;
		return GPUSampler(desc);
	}
}
