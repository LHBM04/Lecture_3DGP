#pragma once

#include <filesystem>
#include <utility>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

#include "../Graphics/GPUGraphicsPipelineState.hpp"

#include "../Math/ColorRGBA.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class TextureAsset;
	class ShaderAsset;

	struct MaterialAssetData final
	{
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK;
		D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;

		ColorRGBA<f32> baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> emissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		ColorRGBA<f32> specularColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		f32 metallic = 0.0f;
		f32 roughness = 0.5f;

		const TextureAsset* baseColorTexture = nullptr;
		const ShaderAsset* vertexShader = nullptr;
		const ShaderAsset* pixelShader = nullptr;

		GPUGraphicsPipelineState pipelineState;
		DXGI_FORMAT targetFormat = DXGI_FORMAT_UNKNOWN;
		bool isSkybox = false;
	};

	class MaterialAsset final : public Asset
	{
	public:
		explicit MaterialAsset(
			std::filesystem::path path,
			MaterialAssetData&& data) noexcept;

		[[nodiscard]] D3D12_CULL_MODE GetCullMode() const noexcept;
		[[nodiscard]] D3D12_FILL_MODE GetFillMode() const noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetBaseColor() const noexcept;
		[[nodiscard]] const ColorRGBA<f32>& GetEmissiveColor() const noexcept;
		[[nodiscard]] const ColorRGBA<f32>& GetSpecularColor() const noexcept;

		[[nodiscard]] f32 GetMetallic() const noexcept;
		[[nodiscard]] f32 GetRoughness() const noexcept;
		[[nodiscard]] f32 GetSmoothness() const noexcept;

		[[nodiscard]] const TextureAsset* GetBaseColorTexture() const noexcept;
		[[nodiscard]] bool IsSkybox() const noexcept;
		[[nodiscard]] const ShaderAsset* GetVertexShader() const noexcept;
		[[nodiscard]] const ShaderAsset* GetPixelShader() const noexcept;

		[[nodiscard]] const GPUGraphicsPipelineState& GetGraphicsPipelineState() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetTargetFormat() const noexcept;

	private:
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK;
		D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;

		ColorRGBA<f32> baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> emissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		ColorRGBA<f32> specularColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		f32 metallic = 0.0f;
		f32 roughness = 0.5f;

		const TextureAsset* baseColorTexture = nullptr;
		const ShaderAsset* vertexShader = nullptr;
		const ShaderAsset* pixelShader = nullptr;

		GPUGraphicsPipelineState pipelineState;
		DXGI_FORMAT targetFormat = DXGI_FORMAT_UNKNOWN;
		bool isSkybox = false;
	};

	inline MaterialAsset::MaterialAsset(
		std::filesystem::path path,
		MaterialAssetData&& data) noexcept
		: Asset(std::move(path))
		, cullMode(data.cullMode)
		, fillMode(data.fillMode)
		, baseColor(data.baseColor)
		, emissiveColor(data.emissiveColor)
		, specularColor(data.specularColor)
		, metallic(data.metallic)
		, roughness(data.roughness)
		, baseColorTexture(data.baseColorTexture)
		, vertexShader(data.vertexShader)
		, pixelShader(data.pixelShader)
		, pipelineState(std::move(data.pipelineState))
		, targetFormat(data.targetFormat)
		, isSkybox(data.isSkybox)
	{
		ASSERT_FMT(vertexShader, "머티리얼의 정점 셰이더 에셋이 nullptr입니다.");
		ASSERT_FMT(pixelShader, "머티리얼의 픽셀 셰이더 에셋이 nullptr입니다.");
	}

	inline D3D12_CULL_MODE MaterialAsset::GetCullMode() const noexcept
	{
		return cullMode;
	}

	inline D3D12_FILL_MODE MaterialAsset::GetFillMode() const noexcept
	{
		return fillMode;
	}

	inline const ColorRGBA<f32>& MaterialAsset::GetBaseColor() const noexcept
	{
		return baseColor;
	}

	inline const ColorRGBA<f32>& MaterialAsset::GetEmissiveColor() const noexcept
	{
		return emissiveColor;
	}

	inline const ColorRGBA<f32>& MaterialAsset::GetSpecularColor() const noexcept
	{
		return specularColor;
	}

	inline f32 MaterialAsset::GetMetallic() const noexcept
	{
		return metallic;
	}

	inline f32 MaterialAsset::GetRoughness() const noexcept
	{
		return roughness;
	}

	inline f32 MaterialAsset::GetSmoothness() const noexcept
	{
		return 1.0f - roughness;
	}

	inline const TextureAsset* MaterialAsset::GetBaseColorTexture() const noexcept
	{
		return baseColorTexture;
	}

	inline bool MaterialAsset::IsSkybox() const noexcept
	{
		return isSkybox;
	}

	inline const ShaderAsset* MaterialAsset::GetVertexShader() const noexcept
	{
		return vertexShader;
	}

	inline const ShaderAsset* MaterialAsset::GetPixelShader() const noexcept
	{
		return pixelShader;
	}

	inline const GPUGraphicsPipelineState& MaterialAsset::GetGraphicsPipelineState() const noexcept
	{
		return pipelineState;
	}

	inline DXGI_FORMAT MaterialAsset::GetTargetFormat() const noexcept
	{
		return targetFormat;
	}
}
