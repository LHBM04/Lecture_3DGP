#pragma once

#include <concepts>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../Core/AppSubsystem.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem : public AppSubsystem
	{
		friend class System;

		template <std::derived_from<Asset> TAsset>
		TAsset* LoadAsset(const std::filesystem::path& path);

		template <std::derived_from<Asset> TAsset>
		const TAsset* LoadAsset(const std::filesystem::path& path) const noexcept;

	private:
		AssetSubsystem() noexcept;

		std::pmr::vector<std::unique_ptr<Asset>> assets;
	};

	inline AssetSubsystem::AssetSubsystem() noexcept
		: AppSubsystem(10) // TODO: 이거 적당한 값 찾기
	{
	}

	template <std::derived_from<Asset> TAsset>
	TAsset* AssetSubsystem::LoadAsset(const std::filesystem::path& path)
	{
		TAsset* result = nullptr;
		return result;
	}

	template <std::derived_from<Asset> TAsset>
	const TAsset* AssetSubsystem::LoadAsset(const std::filesystem::path& path) const noexcept
	{
		TAsset* result = nullptr;
		return result;
	}
}