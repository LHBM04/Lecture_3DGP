#pragma once

#include <d3d12.h>

#include "Asset.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	class MaterialAsset final : public Asset
	{
	public:
		MaterialAsset(
			const ShaderAsset& vertexShader, 
			const ShaderAsset& pixelShader,
			D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
			D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID) noexcept;
		~MaterialAsset() noexcept override = default;

		MaterialAsset(const MaterialAsset&) = delete;
		MaterialAsset& operator=(const MaterialAsset&) = delete;

		MaterialAsset(MaterialAsset&&) noexcept = default;
		MaterialAsset& operator=(MaterialAsset&&) noexcept = default;

		[[nodiscard]] const ShaderAsset& GetVertexShader() const noexcept;
		[[nodiscard]] const ShaderAsset& GetPixelShader() const noexcept;
		[[nodiscard]] D3D12_CULL_MODE GetCullMode() const noexcept;
		[[nodiscard]] D3D12_FILL_MODE GetFillMode() const noexcept;

	private:
		const ShaderAsset* vertexShader;
		const ShaderAsset* pixelShader;
		D3D12_CULL_MODE cullMode;
		D3D12_FILL_MODE fillMode;
	};

	inline MaterialAsset::MaterialAsset(
		const ShaderAsset& vertexShader, const ShaderAsset& pixelShader,
		D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode) noexcept
		: vertexShader(&vertexShader)
		, pixelShader(&pixelShader)
		, cullMode(cullMode)
		, fillMode(fillMode)
	{
	}

	inline const ShaderAsset& MaterialAsset::GetVertexShader() const noexcept
	{
		return *vertexShader;
	}

	inline const ShaderAsset& MaterialAsset::GetPixelShader() const noexcept
	{
		return *pixelShader;
	}

	inline D3D12_CULL_MODE MaterialAsset::GetCullMode() const noexcept
	{
		return cullMode;
	}

	inline D3D12_FILL_MODE MaterialAsset::GetFillMode() const noexcept
	{
		return fillMode;
	}
}
