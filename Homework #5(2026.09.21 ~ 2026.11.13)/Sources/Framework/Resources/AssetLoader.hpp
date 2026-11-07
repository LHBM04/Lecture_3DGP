#pragma once

#include <concepts>
#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "../Core/Common.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	template <std::derived_from<Asset> TAsset = Asset>
	class AssetLoader;

	template <>
	class AssetLoader<Asset>
	{
	public:
		AssetLoader() noexcept = default;
		virtual ~AssetLoader() noexcept = default;

		AssetLoader(const AssetLoader&) noexcept = delete;
		AssetLoader& operator=(const AssetLoader&) noexcept = delete;

		AssetLoader(AssetLoader&&) noexcept = delete;
		AssetLoader& operator=(AssetLoader&&) noexcept = delete;
	};

	template <std::derived_from<Asset> TAsset>
	class AssetLoader : public AssetLoader<>
	{
	public:
		using AssetLoader<>::AssetLoader;

		[[nodiscard]] virtual std::expected<std::unique_ptr<TAsset>, std::string> Load(
			const std::filesystem::path& path) const = 0;
	};
}
