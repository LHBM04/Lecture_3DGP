#pragma once

#include <filesystem>
#include <span>
#include <utility>
#include <vector>

#include "Asset.hpp"
#include "ExportMetadata.hpp"

namespace TUK::Framework
{
	class TextureAsset;

	class FontAsset final : public Asset
	{
	public:
		explicit FontAsset(std::filesystem::path path, FontImportData&& data, std::vector<const TextureAsset*>&& atlases) noexcept;
		[[nodiscard]] const FontImportData& GetData() const noexcept;
		[[nodiscard]] std::span<const TextureAsset* const> GetAtlases() const noexcept;

	private:
		FontImportData data;
		std::vector<const TextureAsset*> atlases;
	};

	inline FontAsset::FontAsset(
		std::filesystem::path path, FontImportData&& data, std::vector<const TextureAsset*>&& atlases) noexcept
		: Asset(std::move(path))
		, data(std::move(data))
		, atlases(std::move(atlases))
	{
	}

	inline const FontImportData& FontAsset::GetData() const noexcept
	{
		return data;
	}

	inline std::span<const TextureAsset* const> FontAsset::GetAtlases() const noexcept
	{
		return atlases;
	}

}
