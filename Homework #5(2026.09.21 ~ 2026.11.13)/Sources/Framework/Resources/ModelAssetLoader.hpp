#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "../Core/Common.hpp"

#include "AssetLoader.hpp"
#include "ModelAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;

	class ModelAssetLoader final : public AssetLoader<ModelAsset>
	{
	public:
		explicit ModelAssetLoader(AssetSubsystem* assets) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<ModelAsset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		AssetSubsystem* assets = nullptr;
	};
}
