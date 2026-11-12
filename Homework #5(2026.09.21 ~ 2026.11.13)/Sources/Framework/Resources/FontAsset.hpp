#pragma once

#include <array>
#include <filesystem>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "../Core/Common.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	struct FontAtlasData final
	{
		i32 index = 0;
		std::string texture;
		i32 width = 0;
		i32 height = 0;
	};

	struct FontCharacterData final
	{
		u32 unicode = 0;
		u32 glyphIndex = 0;
		i32 atlasIndex = 0;
		f32 characterScale = 0.0f;
		f32 glyphScale = 0.0f;
		std::array<f32, 4> glyphRect{};
		std::array<f32, 4> sampleRect{};
		std::array<f32, 2> uvMin{};
		std::array<f32, 2> uvMax{};
		f32 width = 0.0f;
		f32 height = 0.0f;
		f32 bearingX = 0.0f;
		f32 bearingY = 0.0f;
		f32 advance = 0.0f;
	};

	class TextureAsset;

	class FontAsset final : public Asset
	{
	public:
		explicit FontAsset(std::filesystem::path path) noexcept;

		[[nodiscard]] const std::string& GetName() const noexcept;
		void SetName(std::string&& value) noexcept;

		[[nodiscard]] const std::string& GetRenderMode() const noexcept;
		void SetRenderMode(std::string&& value) noexcept;

		[[nodiscard]] bool IsDistanceField() const noexcept;
		void SetDistanceField(bool value) noexcept;

		[[nodiscard]] f32 GetPointSize() const noexcept;
		void SetPointSize(f32 value) noexcept;

		[[nodiscard]] f32 GetFaceScale() const noexcept;
		void SetFaceScale(f32 value) noexcept;

		[[nodiscard]] f32 GetLineHeight() const noexcept;
		void SetLineHeight(f32 value) noexcept;

		[[nodiscard]] f32 GetAscentLine() const noexcept;
		void SetAscentLine(f32 value) noexcept;

		[[nodiscard]] f32 GetDescentLine() const noexcept;
		void SetDescentLine(f32 value) noexcept;

		[[nodiscard]] i32 GetAtlasPadding() const noexcept;
		void SetAtlasPadding(i32 value) noexcept;

		[[nodiscard]] std::span<const FontAtlasData> GetAtlasData() const noexcept;
		void SetAtlasData(std::vector<FontAtlasData>&& value) noexcept;

		[[nodiscard]] std::span<const FontCharacterData> GetCharacters() const noexcept;
		void SetCharacters(std::vector<FontCharacterData>&& value) noexcept;

		[[nodiscard]] std::span<const TextureAsset* const> GetAtlases() const noexcept;
		void SetAtlases(std::vector<const TextureAsset*>&& value) noexcept;

	private:
		std::string name;
		std::string renderMode;
		bool isDistanceField = false;
		f32 pointSize = 0.0f;
		f32 faceScale = 0.0f;
		f32 lineHeight = 0.0f;
		f32 ascentLine = 0.0f;
		f32 descentLine = 0.0f;
		i32 atlasPadding = 0;
		std::vector<FontAtlasData> atlasData;
		std::vector<FontCharacterData> characters;
		std::vector<const TextureAsset*> atlases;
	};

	inline FontAsset::FontAsset(std::filesystem::path path) noexcept
		: Asset(std::move(path))
	{
	}

	inline const std::string& FontAsset::GetName() const noexcept
	{
		return name;
	}

	inline void FontAsset::SetName(std::string&& value) noexcept
	{
		name = std::move(value);
	}

	inline const std::string& FontAsset::GetRenderMode() const noexcept
	{
		return renderMode;
	}

	inline void FontAsset::SetRenderMode(std::string&& value) noexcept
	{
		renderMode = std::move(value);
	}

	inline bool FontAsset::IsDistanceField() const noexcept
	{
		return isDistanceField;
	}

	inline void FontAsset::SetDistanceField(bool value) noexcept
	{
		isDistanceField = value;
	}

	inline f32 FontAsset::GetPointSize() const noexcept
	{
		return pointSize;
	}

	inline void FontAsset::SetPointSize(f32 value) noexcept
	{
		pointSize = value;
	}

	inline f32 FontAsset::GetFaceScale() const noexcept
	{
		return faceScale;
	}

	inline void FontAsset::SetFaceScale(f32 value) noexcept
	{
		faceScale = value;
	}

	inline f32 FontAsset::GetLineHeight() const noexcept
	{
		return lineHeight;
	}

	inline void FontAsset::SetLineHeight(f32 value) noexcept
	{
		lineHeight = value;
	}

	inline f32 FontAsset::GetAscentLine() const noexcept
	{
		return ascentLine;
	}

	inline void FontAsset::SetAscentLine(f32 value) noexcept
	{
		ascentLine = value;
	}

	inline f32 FontAsset::GetDescentLine() const noexcept
	{
		return descentLine;
	}

	inline void FontAsset::SetDescentLine(f32 value) noexcept
	{
		descentLine = value;
	}

	inline i32 FontAsset::GetAtlasPadding() const noexcept
	{
		return atlasPadding;
	}

	inline void FontAsset::SetAtlasPadding(i32 value) noexcept
	{
		atlasPadding = value;
	}

	inline std::span<const FontAtlasData> FontAsset::GetAtlasData() const noexcept
	{
		return atlasData;
	}

	inline void FontAsset::SetAtlasData(std::vector<FontAtlasData>&& value) noexcept
	{
		atlasData = std::move(value);
	}

	inline std::span<const FontCharacterData> FontAsset::GetCharacters() const noexcept
	{
		return characters;
	}

	inline void FontAsset::SetCharacters(std::vector<FontCharacterData>&& value) noexcept
	{
		characters = std::move(value);
	}

	inline std::span<const TextureAsset* const> FontAsset::GetAtlases() const noexcept
	{
		return atlases;
	}

	inline void FontAsset::SetAtlases(std::vector<const TextureAsset*>&& value) noexcept
	{
		atlases = std::move(value);
	}
}
