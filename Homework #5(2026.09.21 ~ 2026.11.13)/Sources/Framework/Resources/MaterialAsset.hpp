#pragma once

#include <algorithm>
#include <cassert>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

#include "../Math/ColorRGBA.hpp"

#include "../Graphics/GPUGraphicsPipelineState.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class MaterialAsset final : public Asset
	{
	public:
		using Asset::Asset;

		MaterialAsset() noexcept = default;

		[[nodiscard]] std::string Serialize() const override;
		[[nodiscard]] bool Deserialize(std::string_view text) override;

		[[nodiscard]] D3D12_CULL_MODE GetCullMode() const noexcept;
		void SetCullMode(D3D12_CULL_MODE value) noexcept;

		[[nodiscard]] D3D12_FILL_MODE GetFillMode() const noexcept;
		void SetFillMode(D3D12_FILL_MODE value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetBaseColor() const noexcept;
		void SetBaseColor(const ColorRGBA<f32>& value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetEmissiveColor() const noexcept;
		void SetEmissiveColor(const ColorRGBA<f32>& value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetSpecularColor() const noexcept;
		void SetSpecularColor(const ColorRGBA<f32>& value) noexcept;

		[[nodiscard]] f32 GetMetallic() const noexcept;
		void SetMetallic(f32 value) noexcept;

		[[nodiscard]] f32 GetRoughness() const noexcept;
		void SetRoughness(f32 value) noexcept;

		[[nodiscard]] f32 GetSmoothness() const noexcept;
		void SetSmoothness(f32 value) noexcept;

		[[nodiscard]] std::string_view GetShaderPath() const noexcept;
		void SetShaderPath(std::string_view value) noexcept;
		[[nodiscard]] std::string_view GetVertexShaderPath() const noexcept;
		[[nodiscard]] std::string_view GetPixelShaderPath() const noexcept;

		[[nodiscard]] const GPUGraphicsPipelineState* GetGraphicsPipelineState() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetTargetFormat() const noexcept;
		void SetGraphicsPipelineState(GPUGraphicsPipelineState value, DXGI_FORMAT format) noexcept;

	private:
		std::optional<GPUGraphicsPipelineState> pipelineState;
		DXGI_FORMAT targetFormat = DXGI_FORMAT_UNKNOWN;
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK;
		D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;

		std::string shaderPath = "Resources/Shaders/Standard.hlsl";
		std::string vertexShaderPath = "Resources/Shaders/Standard.vert.cso";
		std::string pixelShaderPath = "Resources/Shaders/Standard.frag.cso";

		ColorRGBA<f32> baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> emissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		ColorRGBA<f32> specularColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		f32 metallic = 0.0f;
		f32 roughness = 0.5f;
	};

	inline D3D12_CULL_MODE MaterialAsset::GetCullMode() const noexcept
	{
		return cullMode;
	}

	inline void MaterialAsset::SetCullMode(D3D12_CULL_MODE value) noexcept
	{
		cullMode = value;
	}

	inline D3D12_FILL_MODE MaterialAsset::GetFillMode() const noexcept
	{
		return fillMode;
	}

	inline void MaterialAsset::SetFillMode(D3D12_FILL_MODE value) noexcept
	{
		fillMode = value;
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

	inline const ColorRGBA<f32>& MaterialAsset::GetSpecularColor() const noexcept
	{
		return specularColor;
	}

	inline void MaterialAsset::SetSpecularColor(const ColorRGBA<f32>& value) noexcept
	{
		specularColor = value;
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

	inline f32 MaterialAsset::GetSmoothness() const noexcept
	{
		return 1.0f - roughness;
	}

	inline void MaterialAsset::SetSmoothness(f32 value) noexcept
	{
		SetRoughness(1.0f - value);
	}

	inline std::string_view MaterialAsset::GetShaderPath() const noexcept
	{
		return shaderPath;
	}

	inline void MaterialAsset::SetShaderPath(std::string_view value) noexcept
	{
		shaderPath.assign(value);
		std::filesystem::path vertex(shaderPath);
		std::filesystem::path pixel(shaderPath);
		vertex.replace_extension(".vert.cso");
		pixel.replace_extension(".frag.cso");
		vertexShaderPath = vertex.generic_string();
		pixelShaderPath = pixel.generic_string();
	}

	inline std::string_view MaterialAsset::GetVertexShaderPath() const noexcept
	{
		return vertexShaderPath;
	}

	inline std::string_view MaterialAsset::GetPixelShaderPath() const noexcept
	{
		return pixelShaderPath;
	}

	inline const GPUGraphicsPipelineState* MaterialAsset::GetGraphicsPipelineState() const noexcept
	{
		return pipelineState ? &*pipelineState : nullptr;
	}

	inline DXGI_FORMAT MaterialAsset::GetTargetFormat() const noexcept
	{
		return targetFormat;
	}

	inline void MaterialAsset::SetGraphicsPipelineState(GPUGraphicsPipelineState value, DXGI_FORMAT format) noexcept
	{
		pipelineState = std::move(value);
		targetFormat = format;
	}
}
