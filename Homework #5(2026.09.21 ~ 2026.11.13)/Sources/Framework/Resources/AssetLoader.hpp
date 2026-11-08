#pragma once

#include <concepts>
#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "../Core/Common.hpp"

#include "../Utilities/Logger.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;

	template <std::derived_from<Asset> TAsset = Asset>
	class AssetLoader;

	template <>
	class AssetLoader<Asset>
	{
	public:
		explicit AssetLoader(AssetSubsystem* owner) noexcept;
		virtual ~AssetLoader() noexcept = default;

		AssetLoader(const AssetLoader&) noexcept = delete;
		AssetLoader& operator=(const AssetLoader&) noexcept = delete;

		AssetLoader(AssetLoader&&) noexcept = delete;
		AssetLoader& operator=(AssetLoader&&) noexcept = delete;

		[[nodiscard]] AssetSubsystem* GetOwner() const noexcept;

	private:
		AssetSubsystem* owner = nullptr;
	};

	template <std::derived_from<Asset> TAsset>
	class AssetLoader : public AssetLoader<>
	{
	public:
		using AssetLoader<>::AssetLoader;

		[[nodiscard]] virtual std::expected<std::unique_ptr<TAsset>, std::string> Load(
			const std::filesystem::path& path) const = 0;
	};

	inline AssetLoader<Asset>::AssetLoader(AssetSubsystem* owner) noexcept
		: owner(owner)
	{
		ASSERT_FMT(owner, "에셋 로더 소유자가 nullptr입니다.");
	}

	inline AssetSubsystem* AssetLoader<Asset>::GetOwner() const noexcept
	{
		return owner;
	}
}
