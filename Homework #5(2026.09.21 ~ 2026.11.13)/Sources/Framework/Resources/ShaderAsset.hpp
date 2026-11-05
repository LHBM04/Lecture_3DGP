#pragma once

#include <filesystem>
#include <optional>
#include <utility>

#include "../Core/Common.hpp"

#include "../Graphics/GPUShader.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class ShaderAsset : public Asset
	{
	public:
		using Asset::Asset;

		ShaderAsset() noexcept = default;
		ShaderAsset(GPUShader shader, std::filesystem::path sourcePath = {}) noexcept;

		[[nodiscard]] std::string Serialize() const override;
		[[nodiscard]] bool Deserialize(std::string_view text) override;

		[[nodiscard]] bool HasShader() const noexcept;
		[[nodiscard]] const GPUShader* GetShader() const noexcept;
		[[nodiscard]] const std::filesystem::path& GetSourcePath() const noexcept;

	private:
		std::optional<GPUShader> shader;
	};

	inline ShaderAsset::ShaderAsset(GPUShader shader, std::filesystem::path sourcePath) noexcept
		: Asset(std::move(sourcePath))
		, shader(std::move(shader))
	{
	}

	inline bool ShaderAsset::HasShader() const noexcept
	{
		return shader.has_value();
	}

	inline const GPUShader* ShaderAsset::GetShader() const noexcept
	{
		return shader ? &*shader : nullptr;
	}

	inline const std::filesystem::path& ShaderAsset::GetSourcePath() const noexcept
	{
		return GetPath();
	}
}
