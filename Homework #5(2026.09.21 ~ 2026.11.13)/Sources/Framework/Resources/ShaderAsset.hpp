#pragma once

#include <cassert>
#include <filesystem>
#include <optional>
#include <utility>

#include "Framework/Graphics/GPUShader.hpp"

#include "Asset.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class ShaderAsset : public Asset
	{
	public:
		using Asset::Asset;

		ShaderAsset() noexcept = default;
		ShaderAsset(GPUShader shader, std::filesystem::path sourcePath = {}) noexcept;
		~ShaderAsset() noexcept override = default;

		ShaderAsset(const ShaderAsset&) = delete;
		ShaderAsset& operator=(const ShaderAsset&) = delete;

		ShaderAsset(ShaderAsset&&) noexcept = default;
		ShaderAsset& operator=(ShaderAsset&&) noexcept = default;

		[[nodiscard]] std::string Serialize() const override;
		[[nodiscard]] bool Deserialize(std::string_view text) override;

		[[nodiscard]] const GPUShader& GetShader() const noexcept;
		[[nodiscard]] const std::filesystem::path& GetSourcePath() const noexcept;

	private:
		std::optional<GPUShader> shader;
	};

	inline const GPUShader& ShaderAsset::GetShader() const noexcept
	{
		assert(shader);
		return *shader;
	}

	inline ShaderAsset::ShaderAsset(GPUShader shader, std::filesystem::path sourcePath) noexcept
		: Asset(std::move(sourcePath))
		, shader(std::move(shader))
	{
	}
	inline const std::filesystem::path& ShaderAsset::GetSourcePath() const noexcept
	{
		return GetPath();
	}
}
