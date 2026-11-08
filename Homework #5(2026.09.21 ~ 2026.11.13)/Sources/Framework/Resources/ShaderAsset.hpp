#pragma once

#include <filesystem>
#include <utility>

#include "../Core/Common.hpp"

#include "../Graphics/GPUShader.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class ShaderAsset final : public Asset
	{
	public:
		explicit ShaderAsset(std::filesystem::path path, GPUShader&& shader) noexcept;

		[[nodiscard]] const GPUShader& GetShader() const noexcept;

	private:
		GPUShader shader;
	};

	inline ShaderAsset::ShaderAsset(std::filesystem::path path, GPUShader&& shader) noexcept
		: Asset(std::move(path))
		, shader(std::move(shader))
	{
	}

	inline const GPUShader& ShaderAsset::GetShader() const noexcept
	{
		return shader;
	}
}
