#pragma once

#include "Framework/Resources.hpp"

namespace TUK::Game
{
	using namespace Framework;

	namespace Assets
	{
		void Initialize(AssetSubsystem* subsystem) noexcept;
		void Release() noexcept;

		[[nodiscard]] AssetSubsystem* GetSubsystem() noexcept;

		template <std::derived_from<Asset> TAsset>
		[[nodiscard]] const TAsset* LoadAsset(const std::filesystem::path& path);

		void UnloadAsset(const std::filesystem::path& path);
		void UnloadAllAssets() noexcept;
	}

	template <std::derived_from<Asset> TAsset>
	inline const TAsset* Assets::LoadAsset(const std::filesystem::path& path)
	{
		AssetSubsystem* subsystem = GetSubsystem();
		return subsystem ? subsystem->LoadAsset<TAsset>(path) : nullptr;
	}
}
