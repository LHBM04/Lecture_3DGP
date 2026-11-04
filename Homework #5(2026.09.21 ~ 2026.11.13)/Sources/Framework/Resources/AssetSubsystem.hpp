#pragma once

#include <concepts>
#include <expected>
#include <filesystem>
#include <iostream>
#include <memory>
#include <memory_resource>
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
	class AssetSubsystem : public AppSubsystem
	{
		friend class System;

	public:
		AssetSubsystem() noexcept;

		template <std::derived_from<AssetLoader> TAssetLoader, class... TArgs>
		void AddLoader(TArgs&&... args);

		template <std::derived_from<Asset> TAsset>
		TAsset* LoadAsset(const std::filesystem::path& path);

		template <std::derived_from<Asset> TAsset, class... TArgs>
		TAsset* CreateAsset(TArgs&&... args);

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

	private:
		using AssetLoaderList = std::pmr::vector<std::unique_ptr<AssetLoader>>;
		using AssetLoaderDict = std::pmr::unordered_map<std::type_index, AssetLoader*>;

		using AssetList = std::pmr::vector<std::unique_ptr<Asset>>;
		using AssetDict = std::pmr::unordered_map<std::filesystem::path, Asset*>;

		AssetLoaderList assetLoaders;
		AssetLoaderDict assetLoadersByType;

		AssetList assets;
		AssetDict assetsByPath;
	};

	inline AssetSubsystem::AssetSubsystem() noexcept
		: AppSubsystem(10)
	{
	}

	template <std::derived_from<AssetLoader> TAssetLoader, class... TArgs>
	void AssetSubsystem::AddLoader(TArgs&&... args)
	{
		using TAsset = typename TAssetLoader::AssetType;
		static_assert(std::derived_from<TAsset, Asset>);

		const std::type_index assetType(typeid(TAsset));
		if (assetLoadersByType.contains(assetType))
		{
			return;
		}

		auto loader = std::make_unique<TAssetLoader>(std::forward<TArgs>(args)...);
		AssetLoader* pointer = loader.get();
		assetLoaders.emplace_back(std::move(loader));
		assetLoadersByType.emplace(assetType, pointer);
	}

	template <std::derived_from<Asset> TAsset>
	TAsset* AssetSubsystem::LoadAsset(const std::filesystem::path& path)
	{
		const std::filesystem::path normalizedPath = path.lexically_normal();
		if (const auto cached = assetsByPath.find(normalizedPath); cached != assetsByPath.end())
		{
			return dynamic_cast<TAsset*>(cached->second);
		}

		const auto assetLoader = assetLoadersByType.find(std::type_index(typeid(TAsset)));
		if (assetLoader == assetLoadersByType.end())
		{
			return nullptr;
		}

		auto asset = assetLoader->second->Load(normalizedPath);
		if (!asset)
		{
			std::cerr << "Asset load failed: " << normalizedPath.generic_string() << ": " << asset.error() << std::endl;
			return nullptr;
		}

		auto* result = dynamic_cast<TAsset*>(asset->get());
		if (!result)
		{
			return nullptr;
		}

		assets.emplace_back(std::move(*asset));
		assetsByPath.emplace(normalizedPath, result);
		return result;
	}

	template <std::derived_from<Asset> TAsset, class... TArgs>
	TAsset* AssetSubsystem::CreateAsset(TArgs&&... args)
	{
		auto asset = std::make_unique<TAsset>(std::forward<TArgs>(args)...);
		TAsset* result = asset.get();
		assets.emplace_back(std::move(asset));
		return result;
	}
}
