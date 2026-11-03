#pragma once

#include <utility>

#include "Framework/Graphics/GPUShader.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class ShaderAsset : public Asset
	{
	public:
		ShaderAsset(GPUShader shader) noexcept;
		~ShaderAsset() noexcept override = default;

		ShaderAsset(const ShaderAsset&) = delete;
		ShaderAsset& operator=(const ShaderAsset&) = delete;

		ShaderAsset(ShaderAsset&&) noexcept = default;
		ShaderAsset& operator=(ShaderAsset&&) noexcept = default;

		[[nodiscard]] const GPUShader& GetShader() const noexcept;

	private:
		GPUShader shader;
	};

	inline const GPUShader& ShaderAsset::GetShader() const noexcept
	{
		return shader;
	}

	inline ShaderAsset::ShaderAsset(GPUShader shader) noexcept
		: shader(std::move(shader))
	{
	}
}
