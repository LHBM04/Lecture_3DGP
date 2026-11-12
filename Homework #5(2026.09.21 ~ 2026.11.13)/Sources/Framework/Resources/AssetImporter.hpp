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
	class AssetSubsystem;

	template <std::derived_from<Asset> TAsset = Asset>
	class AssetImporter;

	template <>
	class AssetImporter<Asset>
	{
	public:
		explicit AssetImporter(AssetSubsystem* owner) noexcept;
		virtual ~AssetImporter() noexcept = default;

		AssetImporter(const AssetImporter&) noexcept = delete;
		AssetImporter& operator=(const AssetImporter&) noexcept = delete;

		AssetImporter(AssetImporter&&) noexcept = delete;
		AssetImporter& operator=(AssetImporter&&) noexcept = delete;

		[[nodiscard]] AssetSubsystem* GetOwner() const noexcept;

	private:
		AssetSubsystem* owner = nullptr;
	};

	template <std::derived_from<Asset> TAsset>
	class AssetImporter : public AssetImporter<>
	{
	public:
		using AssetImporter<>::AssetImporter;

		[[nodiscard]] virtual std::expected<std::unique_ptr<TAsset>, std::string> Import(
			const std::filesystem::path& path) const = 0;
	};

	inline AssetImporter<Asset>::AssetImporter(AssetSubsystem* owner) noexcept
		: owner(owner)
	{
		ASSERT_FMT(owner, "에셋 임포터 소유자가 nullptr입니다.");
	}

	inline AssetSubsystem* AssetImporter<Asset>::GetOwner() const noexcept
	{
		return owner;
	}
}
