#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "Asset.hpp"

namespace TUK::Framework
{
	class AssetLoader
	{
	public:
		virtual ~AssetLoader() noexcept = default;

		AssetLoader(const AssetLoader&) noexcept = delete;
		AssetLoader& operator=(const AssetLoader&) noexcept = delete;

		AssetLoader(AssetLoader&&) noexcept = delete;
		AssetLoader& operator=(AssetLoader&&) noexcept = delete;

		[[nodiscard]] virtual std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const = 0;

	protected:
		AssetLoader() noexcept = default;
	};
}
