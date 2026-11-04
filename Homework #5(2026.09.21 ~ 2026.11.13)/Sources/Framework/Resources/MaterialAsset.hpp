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

#include "../Graphics/GPUGraphicsPipelineState.hpp"

#include "../Math/ColorRGBA.hpp"

#include "Asset.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;
	class ShaderAsset;

	class MaterialAsset final : public Asset
	{
	public:
		MaterialAsset() noexcept = default;
		~MaterialAsset() noexcept override = default;

		MaterialAsset(const MaterialAsset&) = delete;
		MaterialAsset& operator=(const MaterialAsset&) = delete;

		MaterialAsset(MaterialAsset&&) noexcept = default;
		MaterialAsset& operator=(MaterialAsset&&) noexcept = default;

		[[nodiscard]] std::string Serialize() const override;
		[[nodiscard]] bool Deserialize(std::string_view text) override;
		[[nodiscard]] bool Upload(
			const GPUDevice& device,
			AssetSubsystem& assets,
			const std::filesystem::path& path,
			DXGI_FORMAT format);

		[[nodiscard]] static std::expected<MaterialAsset, std::string> Create(
			const GPUDevice& device,
			const ShaderAsset& vertexShader,
			const ShaderAsset& pixelShader,
			DXGI_FORMAT targetFormat,
			D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
			D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID);

		[[nodiscard]] D3D12_CULL_MODE GetCullMode() const noexcept;
		[[nodiscard]] D3D12_FILL_MODE GetFillMode() const noexcept;
		[[nodiscard]] const GPUGraphicsPipelineState& GetGraphicsPipelineState() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetTargetFormat() const noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetAlbedoColor() const noexcept;
		void SetAlbedoColor(const ColorRGBA<f32>& value) noexcept;
		[[nodiscard]] const ColorRGBA<f32>& GetBaseColor() const noexcept;
		void SetBaseColor(const ColorRGBA<f32>& value) noexcept;
		[[nodiscard]] const ColorRGBA<f32>& GetEmissiveColor() const noexcept;
		void SetEmissiveColor(const ColorRGBA<f32>& value) noexcept;
		[[nodiscard]] const ColorRGBA<f32>& GetSpecularColor() const noexcept;
		void SetSpecularColor(const ColorRGBA<f32>& value) noexcept;

		[[nodiscard]] f32 GetGlossiness() const noexcept;
		void SetGlossiness(f32 value) noexcept;
		[[nodiscard]] f32 GetSmoothness() const noexcept;
		void SetSmoothness(f32 value) noexcept;
		[[nodiscard]] f32 GetMetallic() const noexcept;
		void SetMetallic(f32 value) noexcept;
		[[nodiscard]] f32 GetSpecularHighlight() const noexcept;
		void SetSpecularHighlight(f32 value) noexcept;
		[[nodiscard]] f32 GetGlossyReflection() const noexcept;
		void SetGlossyReflection(f32 value) noexcept;
		[[nodiscard]] f32 GetRoughness() const noexcept;
		void SetRoughness(f32 value) noexcept;

		[[nodiscard]] std::string_view GetShaderPath() const noexcept;
		void SetShaderPath(std::string_view value) noexcept;

	private:
		explicit MaterialAsset(
			GPUGraphicsPipelineState graphicsPipelineState,
			DXGI_FORMAT targetFormat,
			D3D12_CULL_MODE cullMode,
			D3D12_FILL_MODE fillMode) noexcept;

		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK;
		D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;
		std::optional<GPUGraphicsPipelineState> graphicsPipelineState;
		DXGI_FORMAT targetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		std::string shaderPath = "Resources/Shaders/Standard.hlsl";
		std::string vertexShaderPath = "Resources/Shaders/Standard.vert.hlsl";
		std::string pixelShaderPath = "Resources/Shaders/Standard.frag.hlsl";
		ColorRGBA<f32> albedoColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> emissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		ColorRGBA<f32> specularColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		f32 glossiness = 0.5f;
		f32 smoothness = 0.5f;
		f32 metallic = 0.0f;
		f32 specularHighlight = 0.0f;
		f32 glossyReflection = 0.0f;
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
		assert(graphicsPipelineState);
		return *graphicsPipelineState;
	}

	inline DXGI_FORMAT MaterialAsset::GetTargetFormat() const noexcept
	{
		return targetFormat;
	}

	inline const ColorRGBA<f32>& MaterialAsset::GetAlbedoColor() const noexcept
	{
		return albedoColor;
	}

	inline void MaterialAsset::SetAlbedoColor(const ColorRGBA<f32>& value) noexcept
	{
		albedoColor = value;
	}

	inline const ColorRGBA<f32>& MaterialAsset::GetBaseColor() const noexcept
	{
		return albedoColor;
	}

	inline void MaterialAsset::SetBaseColor(const ColorRGBA<f32>& value) noexcept
	{
		albedoColor = value;
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

	inline f32 MaterialAsset::GetGlossiness() const noexcept
	{
		return glossiness;
	}

	inline void MaterialAsset::SetGlossiness(f32 value) noexcept
	{
		glossiness = std::clamp(value, 0.0f, 1.0f);
	}

	inline f32 MaterialAsset::GetSmoothness() const noexcept
	{
		return smoothness;
	}

	inline void MaterialAsset::SetSmoothness(f32 value) noexcept
	{
		smoothness = std::clamp(value, 0.0f, 1.0f);
		roughness = 1.0f - smoothness;
	}

	inline f32 MaterialAsset::GetMetallic() const noexcept
	{
		return metallic;
	}

	inline void MaterialAsset::SetMetallic(f32 value) noexcept
	{
		metallic = std::clamp(value, 0.0f, 1.0f);
	}

	inline f32 MaterialAsset::GetSpecularHighlight() const noexcept
	{
		return specularHighlight;
	}

	inline void MaterialAsset::SetSpecularHighlight(f32 value) noexcept
	{
		specularHighlight = std::clamp(value, 0.0f, 1.0f);
	}

	inline f32 MaterialAsset::GetGlossyReflection() const noexcept
	{
		return glossyReflection;
	}

	inline void MaterialAsset::SetGlossyReflection(f32 value) noexcept
	{
		glossyReflection = std::clamp(value, 0.0f, 1.0f);
	}

	inline f32 MaterialAsset::GetRoughness() const noexcept
	{
		return roughness;
	}

	inline void MaterialAsset::SetRoughness(f32 value) noexcept
	{
		roughness = std::clamp(value, 0.0f, 1.0f);
		smoothness = 1.0f - roughness;
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
		vertex.replace_extension(".vert.hlsl");
		pixel.replace_extension(".frag.hlsl");
		vertexShaderPath = vertex.generic_string();
		pixelShaderPath = pixel.generic_string();
	}

	inline MaterialAsset::MaterialAsset(
		GPUGraphicsPipelineState graphicsPipelineState,
		DXGI_FORMAT targetFormat,
		D3D12_CULL_MODE cullMode,
		D3D12_FILL_MODE fillMode) noexcept
		: cullMode(cullMode)
		, fillMode(fillMode)
		, graphicsPipelineState(std::move(graphicsPipelineState))
		, targetFormat(targetFormat)
	{
	}
}
