#pragma once

#include <concepts>
#include <expected>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../Core/AppSubsystem.hpp"
#include "../Core/Common.hpp"

#include "Asset.hpp"
#include "AssetLoader.hpp"

namespace TUK::Framework
{
	class World;

	class AssetSubsystem : public AppSubsystem
	{
	public:
		AssetSubsystem() noexcept;

		template <std::derived_from<AssetLoader<>> TAssetLoader, class... TArgs>
		void AddLoader(TArgs&&... args);

		template <std::derived_from<Asset> TAsset>
		[[nodiscard]] const TAsset* FindLoadedAsset(const std::filesystem::path& path) const noexcept;

		template <std::derived_from<Asset> TAsset>
		[[nodiscard]] const TAsset* LoadAsset(const std::filesystem::path& path);

		void UnloadAsset(const std::filesystem::path& path);
		void UnloadAllAssets() noexcept;

		void OnWorldEntered(const World* world);
		void OnWorldExited(const World* world);

		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

	private:
		using AssetLoaderList = std::vector<std::unique_ptr<AssetLoader<>>>;
		using AssetLoaderDict = std::unordered_map<std::type_index, AssetLoader<>*>;

		using AssetList = std::vector<std::unique_ptr<Asset>>;
		using AssetDict = std::unordered_map<std::filesystem::path, const Asset*>;

		AssetLoaderList assetLoaders;
		AssetLoaderDict assetLoadersByType;

		AssetList assets;
		AssetDict assetsByPath;
	};

	inline AssetSubsystem::AssetSubsystem() noexcept
		: AppSubsystem(10)
	{
	}

	template <std::derived_from<AssetLoader<>> TAssetLoader, class... TArgs>
	void AssetSubsystem::AddLoader(TArgs&&... args)
	{
		const auto getAssetType = []<class TLoader, std::derived_from<Asset> TAsset>(
			std::expected<std::unique_ptr<TAsset>, std::string>(TLoader::*)(const std::filesystem::path&) const) noexcept
		{
			return std::type_index(typeid(TAsset));
		};

		const std::type_index assetType = getAssetType(&TAssetLoader::Load);
		if (assetLoadersByType.contains(assetType))
		{
			return;
		}

		auto loader = std::make_unique<TAssetLoader>(this, std::forward<TArgs>(args)...);
		AssetLoader<>* pointer = loader.get();
		assetLoaders.emplace_back(std::move(loader));
		assetLoadersByType.emplace(assetType, pointer);
	}

	template <std::derived_from<Asset> TAsset>
	const TAsset* AssetSubsystem::FindLoadedAsset(const std::filesystem::path& path) const noexcept
	{
		const auto cached = assetsByPath.find(path.lexically_normal());
		if (cached == assetsByPath.end())
		{
			return nullptr;
		}
		return dynamic_cast<const TAsset*>(cached->second);
	}

	template <std::derived_from<Asset> TAsset>
	const TAsset* AssetSubsystem::LoadAsset(const std::filesystem::path& path)
	{
		const std::filesystem::path normalizedPath = path.lexically_normal();
		if (const auto cached = assetsByPath.find(normalizedPath); cached != assetsByPath.end())
		{
			return dynamic_cast<const TAsset*>(cached->second);
		}

		const auto assetLoader = assetLoadersByType.find(std::type_index(typeid(TAsset)));
		if (assetLoader == assetLoadersByType.end())
		{
			return nullptr;
		}

		const auto* loader = static_cast<const AssetLoader<TAsset>*>(assetLoader->second);
		auto asset = loader->Load(normalizedPath);
		if (!asset)
		{
			LOG_ERROR("에셋 로드에 실패했습니다: {}: {}", normalizedPath.generic_string(), asset.error());
			return nullptr;
		}

		const TAsset* result = asset->get();
		if (!result)
		{
			return nullptr;
		}

		assets.emplace_back(std::move(*asset));
		assetsByPath.emplace(normalizedPath, result);
		return result;
	}
}
