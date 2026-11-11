#pragma once

#include <filesystem>
#include <utility>

#include "Asset.hpp"
#include "ExportMetadata.hpp"

namespace TUK::Framework
{
	class AnimationAsset final : public Asset
	{
	public:
		explicit AnimationAsset(std::filesystem::path path, AnimationMetadata&& data) noexcept;
		[[nodiscard]] const AnimationMetadata& GetData() const noexcept;

	private:
		AnimationMetadata data;
	};

	inline AnimationAsset::AnimationAsset(std::filesystem::path path, AnimationMetadata&& data) noexcept
		: Asset(std::move(path))
		, data(std::move(data))
	{
	}

	inline const AnimationMetadata& AnimationAsset::GetData() const noexcept
	{
		return data;
	}
}
