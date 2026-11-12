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

#include <directx/d3d12.h>

#include "../Core/AppSubsystem.hpp"
#include "../Core/Common.hpp"

#include "Asset.hpp"
#include "AssetImporter.hpp"

namespace TUK::Framework
{
	class MaterialAsset;
	class Scene;

	class AssetSubsystem : public AppSubsystem
	{
	public:
		AssetSubsystem() noexcept;

		template <std::derived_from<AssetImporter<>> TAssetImporter, class... TArgs>
		void AddImporter(TArgs&&... args);

		template <std::derived_from<Asset> TAsset>
		[[nodiscard]] const TAsset* LoadAsset(const std::filesystem::path& path);

		void UnloadAsset(const std::filesystem::path& path);
		void UnloadAllAssets() noexcept;

		void OnSceneEntered(const Scene* scene);
		void OnSceneExited(const Scene* scene);

		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

	private:
		template <std::derived_from<Asset> TAsset>
		[[nodiscard]] const TAsset* GetLoadedAsset(const std::filesystem::path& path) const noexcept;

		using AssetImporterList = std::vector<std::unique_ptr<AssetImporter<>>>;
		using AssetImporterDict = std::unordered_map<std::type_index, AssetImporter<>*>;

		using AssetList = std::vector<std::unique_ptr<Asset>>;
		using AssetDict = std::unordered_map<std::filesystem::path, const Asset*>;

		AssetImporterList assetImporters;
		AssetImporterDict assetImportersByType;

		AssetList assets;
		AssetDict assetsByPath;
	};

	inline AssetSubsystem::AssetSubsystem() noexcept
		: AppSubsystem(10)
	{
	}

	template <std::derived_from<AssetImporter<>> TAssetImporter, class... TArgs>
	void AssetSubsystem::AddImporter(TArgs&&... args)
	{
		const auto getAssetType = []<class TImporter, std::derived_from<Asset> TAsset>(
			std::expected<std::unique_ptr<TAsset>, std::string>(TImporter::*)(const std::filesystem::path&) const) noexcept
		{
			return std::type_index(typeid(TAsset));
		};

		const std::type_index assetType = getAssetType(&TAssetImporter::Import);
		if (assetImportersByType.contains(assetType))
		{
			return;
		}

		auto importer = std::make_unique<TAssetImporter>(this, std::forward<TArgs>(args)...);
		AssetImporter<>* pointer = importer.get();
		assetImporters.emplace_back(std::move(importer));
		assetImportersByType.emplace(assetType, pointer);
	}

	template <std::derived_from<Asset> TAsset>
	const TAsset* AssetSubsystem::GetLoadedAsset(const std::filesystem::path& path) const noexcept
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

		const auto assetImporter = assetImportersByType.find(std::type_index(typeid(TAsset)));
		if (assetImporter == assetImportersByType.end())
		{
			return nullptr;
		}

		const auto* importer = static_cast<const AssetImporter<TAsset>*>(assetImporter->second);
		auto asset = importer->Import(normalizedPath);
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
