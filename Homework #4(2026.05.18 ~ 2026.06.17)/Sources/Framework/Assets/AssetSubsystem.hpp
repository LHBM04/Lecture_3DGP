#pragma once

#include <concepts>
#include <expected>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../Core/AppSubsystem.hpp"

#include "Asset.hpp"
#include "AssetLoader.hpp"

namespace TUK::Framework
{
	class AssetSubsystem : public AppSubsystem
	{
		friend class System;

	public:
		~AssetSubsystem() noexcept override = default;

		AssetSubsystem(const AssetSubsystem&) = delete;
		AssetSubsystem& operator=(const AssetSubsystem&) = delete;

		AssetSubsystem(AssetSubsystem&&) noexcept = delete;
		AssetSubsystem& operator=(AssetSubsystem&&) noexcept = delete;

		template <std::derived_from<AssetLoader> TAssetLoader, class... TArgs>
		void AddLoader(TArgs&&... args);

		template <std::derived_from<Asset> TAsset>
		TAsset* LoadAsset(const std::filesystem::path& path);

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
		using AssetDict = std::pmr::unordered_map<std::filesystem::path, AssetList>;

		AssetSubsystem() noexcept;

		AssetLoaderList assetLoaders;
		AssetLoaderDict assetLoadersByType;

		AssetList assets;
	};

	inline AssetSubsystem::AssetSubsystem() noexcept
		: AppSubsystem(10) // TODO: 이거 적당한 값 찾기
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
		const auto assetLoader = assetLoadersByType.find(std::type_index(typeid(TAsset)));
		if (assetLoader == assetLoadersByType.end())
		{
			return nullptr;
		}

		auto asset = assetLoader->second->Load(path);
		if (!asset)
		{
			return nullptr;
		}

		auto* result = dynamic_cast<TAsset*>(asset->get());
		if (!result)
		{
			return nullptr;
		}

		assets.emplace_back(std::move(*asset));
		return result;
	}
}
