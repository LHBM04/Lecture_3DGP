#pragma once

#include <utility>

#include "Asset.hpp"
#include "Framework/Graphics/GPUShader.hpp"

namespace TUK::Framework
{
	class ShaderAsset : public Asset
	{
	public:
		ShaderAsset(GPUShader shader, GPUShaderCreateInfo createInfo) noexcept;
		~ShaderAsset() noexcept override = default;

		ShaderAsset(const ShaderAsset&) = delete;
		ShaderAsset& operator=(const ShaderAsset&) = delete;

		ShaderAsset(ShaderAsset&&) noexcept = default;
		ShaderAsset& operator=(ShaderAsset&&) noexcept = default;

		[[nodiscard]] const GPUShader& GetShader() const noexcept;
		[[nodiscard]] const GPUShaderCreateInfo& GetCreateInfo() const noexcept;

	private:
		GPUShader shader;
		GPUShaderCreateInfo createInfo;
	};

	inline const GPUShader& ShaderAsset::GetShader() const noexcept
	{
		return shader;
	}

	inline const GPUShaderCreateInfo& ShaderAsset::GetCreateInfo() const noexcept
	{
		return createInfo;
	}

	inline ShaderAsset::ShaderAsset(GPUShader shader, GPUShaderCreateInfo createInfo) noexcept
		: shader(std::move(shader))
		, createInfo(std::move(createInfo))
	{
	}
}
