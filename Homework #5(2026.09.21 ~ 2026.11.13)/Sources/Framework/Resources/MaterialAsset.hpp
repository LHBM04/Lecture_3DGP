#pragma once

#include <array>
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

	enum class MaterialTextureSlot : u32
	{
		BASE_COLOR,
		NORMAL,
		METALLIC,
		ROUGHNESS,
		OCCLUSION,
		EMISSIVE,
		SPECULAR,
		MASK,
		COUNT
	};

	using MaterialTextures = std::array<const TextureAsset*, static_cast<usize>(MaterialTextureSlot::COUNT)>;

	class MaterialAsset final : public Asset
	{
	public:
		explicit MaterialAsset(
			std::filesystem::path path,
			const ShaderAsset* vertexShader,
			const ShaderAsset* pixelShader,
			GPUGraphicsPipelineState&& pipelineState) noexcept;

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

		[[nodiscard]] const MaterialTextures& GetTextures() const noexcept;
		void SetTextures(const MaterialTextures& value) noexcept;

		[[nodiscard]] D3D_PRIMITIVE_TOPOLOGY GetPrimitive() const noexcept;
		void SetPrimitive(D3D_PRIMITIVE_TOPOLOGY value) noexcept;

		[[nodiscard]] const ShaderAsset* GetVertexShader() const noexcept;
		[[nodiscard]] const ShaderAsset* GetPixelShader() const noexcept;

		[[nodiscard]] const GPUGraphicsPipelineState& GetGraphicsPipelineState() const noexcept;

		[[nodiscard]] DXGI_FORMAT GetTargetFormat() const noexcept;
		void SetTargetFormat(DXGI_FORMAT value) noexcept;

	private:
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK;
		D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;

		ColorRGBA<f32> baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> emissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		ColorRGBA<f32> specularColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		f32 metallic = 0.0f;
		f32 roughness = 0.5f;

		MaterialTextures textures{};
		const ShaderAsset* vertexShader = nullptr;
		const ShaderAsset* pixelShader = nullptr;

		GPUGraphicsPipelineState pipelineState;
		DXGI_FORMAT targetFormat = DXGI_FORMAT_UNKNOWN;
		D3D_PRIMITIVE_TOPOLOGY primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};

	inline MaterialAsset::MaterialAsset(
		std::filesystem::path path,
		const ShaderAsset* vertexShader,
		const ShaderAsset* pixelShader,
		GPUGraphicsPipelineState&& pipelineState) noexcept
		: Asset(std::move(path))
		, vertexShader(vertexShader)
		, pixelShader(pixelShader)
		, pipelineState(std::move(pipelineState))
	{
		ASSERT_FMT(this->vertexShader, "머티리얼의 정점 셰이더 에셋이 nullptr입니다.");
		ASSERT_FMT(this->pixelShader, "머티리얼의 픽셀 셰이더 에셋이 nullptr입니다.");
	}

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
		metallic = value;
	}

	inline f32 MaterialAsset::GetRoughness() const noexcept
	{
		return roughness;
	}

	inline void MaterialAsset::SetRoughness(f32 value) noexcept
	{
		roughness = value;
	}

	inline f32 MaterialAsset::GetSmoothness() const noexcept
	{
		return 1.0f - roughness;
	}

	inline void MaterialAsset::SetSmoothness(f32 value) noexcept
	{
		roughness = 1.0f - value;
	}

	inline const MaterialTextures& MaterialAsset::GetTextures() const noexcept
	{
		return textures;
	}

	inline void MaterialAsset::SetTextures(const MaterialTextures& value) noexcept
	{
		textures = value;
	}

	inline D3D_PRIMITIVE_TOPOLOGY MaterialAsset::GetPrimitive() const noexcept
	{
		return primitive;
	}

	inline void MaterialAsset::SetPrimitive(D3D_PRIMITIVE_TOPOLOGY value) noexcept
	{
		primitive = value;
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

	inline void MaterialAsset::SetTargetFormat(DXGI_FORMAT value) noexcept
	{
		targetFormat = value;
	}
}
