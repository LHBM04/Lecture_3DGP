#pragma once

#include <algorithm>
#include <expected>
#include <string>
#include <utility>

#include <directx/d3d12.h>

#include "../Graphics/GPUGraphicsPipelineState.hpp"

#include "../Math/ColorRGBA.hpp"

#include "Asset.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUDevice;
	class ShaderAsset;

	class MaterialAsset final : public Asset
	{
	public:
		~MaterialAsset() noexcept override = default;

		MaterialAsset(const MaterialAsset&) = delete;
		MaterialAsset& operator=(const MaterialAsset&) = delete;

		MaterialAsset(MaterialAsset&&) noexcept = default;
		MaterialAsset& operator=(MaterialAsset&&) noexcept = default;

		[[nodiscard]] static std::expected<MaterialAsset, std::string> Create(
			const GPUDevice& device,
			const ShaderAsset& vertexShader, const ShaderAsset& pixelShader,
			DXGI_FORMAT targetFormat,
			D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
			D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID);

		[[nodiscard]] D3D12_CULL_MODE GetCullMode() const noexcept;
		[[nodiscard]] D3D12_FILL_MODE GetFillMode() const noexcept;
		[[nodiscard]] const GPUGraphicsPipelineState& GetGraphicsPipelineState() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetTargetFormat() const noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetBaseColor() const noexcept;
		void SetBaseColor(const ColorRGBA<f32>& value) noexcept;
		[[nodiscard]] const ColorRGBA<f32>& GetEmissiveColor() const noexcept;
		void SetEmissiveColor(const ColorRGBA<f32>& value) noexcept;
		[[nodiscard]] f32 GetMetallic() const noexcept;
		void SetMetallic(f32 value) noexcept;
		[[nodiscard]] f32 GetRoughness() const noexcept;
		void SetRoughness(f32 value) noexcept;

	private:
		explicit MaterialAsset(
			GPUGraphicsPipelineState graphicsPipelineState,
			DXGI_FORMAT targetFormat,
			D3D12_CULL_MODE cullMode, 
			D3D12_FILL_MODE fillMode) noexcept;

		D3D12_CULL_MODE cullMode;
		D3D12_FILL_MODE fillMode;
		GPUGraphicsPipelineState graphicsPipelineState;
		DXGI_FORMAT targetFormat;
		ColorRGBA<f32> baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> emissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		f32 metallic = 0.0f;
		f32 roughness = 0.5f;
	};

	inline D3D12_CULL_MODE MaterialAsset::GetCullMode() const noexcept
	{
		return cullMode;
	}

	inline D3D12_FILL_MODE MaterialAsset::GetFillMode() const noexcept
	{
		return fillMode;
	}

	inline const GPUGraphicsPipelineState& MaterialAsset::GetGraphicsPipelineState() const noexcept
	{
		return graphicsPipelineState;
	}

	inline DXGI_FORMAT MaterialAsset::GetTargetFormat() const noexcept
	{
		return targetFormat;
	}

	inline const ColorRGBA<f32>& MaterialAsset::GetBaseColor() const noexcept
	{
		return baseColor;
	}

	inline void MaterialAsset::SetBaseColor(const ColorRGBA<f32>& value) noexcept
	{
		baseColor = value;
	}

	inline const ColorRGBA<f32>& MaterialAsset::GetEmissiveColor() const noexcept
	{
		return emissiveColor;
	}

	inline void MaterialAsset::SetEmissiveColor(const ColorRGBA<f32>& value) noexcept
	{
		emissiveColor = value;
	}

	inline f32 MaterialAsset::GetMetallic() const noexcept
	{
		return metallic;
	}

	inline void MaterialAsset::SetMetallic(f32 value) noexcept
	{
		metallic = std::clamp(value, 0.0f, 1.0f);
	}

	inline f32 MaterialAsset::GetRoughness() const noexcept
	{
		return roughness;
	}

	inline void MaterialAsset::SetRoughness(f32 value) noexcept
	{
		roughness = std::clamp(value, 0.0f, 1.0f);
	}

	inline MaterialAsset::MaterialAsset(
		GPUGraphicsPipelineState graphicsPipelineState, DXGI_FORMAT targetFormat,
		D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode) noexcept
		: cullMode(cullMode)
		, fillMode(fillMode)
		, graphicsPipelineState(std::move(graphicsPipelineState))
		, targetFormat(targetFormat)
	{
	}
}
