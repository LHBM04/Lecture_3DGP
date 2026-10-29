#pragma once

#include <concepts>
#include <expected>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Core/AppSubsystem.hpp"

#include "Asset.hpp"
#include "ShaderAsset.hpp"

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

		template <std::derived_from<Asset> TAsset>
		TAsset* LoadAsset(const std::filesystem::path& path);

		template <std::derived_from<Asset> TAsset>
		const TAsset* LoadAsset(const std::filesystem::path& path) const noexcept;

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

	private:
		AssetSubsystem() noexcept;

		std::expected<ShaderAsset, std::string> LoadShaderAsset(const std::filesystem::path& path);

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
		if constexpr (std::is_same_v<TAsset, ShaderAsset>)
		{
			std::expected<ShaderAsset, std::string> shaderAsset = LoadShaderAsset(path);
			if (shaderAsset)
			{
				result = (TAsset*)assets.emplace_back(std::make_unique<ShaderAsset>(std::move(*shaderAsset))).get();
			}
			else
			{
				std::cerr << "ShaderAsset: " << path << ": " << shaderAsset.error() << std::endl;
			}
		}

		return result;
	}

	template <std::derived_from<Asset> TAsset>
	const TAsset* AssetSubsystem::LoadAsset(const std::filesystem::path& path) const noexcept
	{
		return static_cast<const TAsset*>(LoadAsset<TAsset>(path));
	}
}
