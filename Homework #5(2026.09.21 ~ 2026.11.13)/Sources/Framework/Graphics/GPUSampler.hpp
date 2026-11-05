#pragma once

#include <directx/d3d12.h>
#include <directx/d3dx12.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	enum class TextureFilter
	{
		Point,
		Linear,
		Anisotropic
	};

	enum class TextureAddressMode
	{
		Wrap,
		Clamp,
		Mirror,
		Border
	};

	class GPUSampler
	{
	public:
		explicit GPUSampler(const D3D12_SAMPLER_DESC& description) noexcept;
		~GPUSampler() noexcept = default;

		GPUSampler(const GPUSampler&) noexcept = default;
		GPUSampler& operator=(const GPUSampler&) noexcept = default;

		GPUSampler(GPUSampler&&) noexcept = default;
		GPUSampler& operator=(GPUSampler&&) noexcept = default;

		[[nodiscard]] const D3D12_SAMPLER_DESC& GetDescription() const noexcept;

		[[nodiscard]] CD3DX12_STATIC_SAMPLER_DESC ToStaticDescription(
			u32 shaderRegister,
			u32 registerSpace = 0,
			D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_PIXEL) const noexcept;

		[[nodiscard]] static GPUSampler LinearWrap() noexcept;
		[[nodiscard]] static GPUSampler LinearClamp() noexcept;
		[[nodiscard]] static GPUSampler PointClamp() noexcept;
		[[nodiscard]] static GPUSampler PointWrap() noexcept;
		[[nodiscard]] static GPUSampler AnisotropicWrap(u32 maxAnisotropy = 16) noexcept;

	private:
		D3D12_SAMPLER_DESC description{};
	};

	inline GPUSampler::GPUSampler(const D3D12_SAMPLER_DESC& description) noexcept
		: description(description)
	{
	}

	inline const D3D12_SAMPLER_DESC& GPUSampler::GetDescription() const noexcept
	{
		return description;
	}
}
