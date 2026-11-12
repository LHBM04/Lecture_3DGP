#include "Precompiled.hpp"
#include "BitmapFontAssetImporter.hpp"

#include "../Core/BinaryReader.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<BitmapFontAsset>, std::string> BitmapFontAssetImporter::Import(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".tukfont")
		{
			return std::unexpected("Font requires a .tukfont file");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("Cannot open resource: " + path.generic_string());
		}

		BinaryReader reader(file);
		std::string name;
		std::string renderMode;
		bool isDistanceField = false;
		f32 pointSize = 0.0f;
		f32 faceScale = 0.0f;
		f32 lineHeight = 0.0f;
		f32 ascentLine = 0.0f;
		f32 descentLine = 0.0f;
		i32 atlasPadding = 0;
		if (!reader.Read(name, renderMode, isDistanceField, pointSize,
			faceScale, lineHeight, ascentLine, descentLine, atlasPadding))
		{
			return std::unexpected("Invalid font header");
		}

		i32 count = 0;
		if (!reader.ReadCount(count, sizeof(FontAtlasData)))
		{
			return std::unexpected("Invalid font atlas count");
		}

		std::vector<FontAtlasData> parsedAtlases(count);
		for (auto& atlas : parsedAtlases)
		{
			if (!reader.Read(atlas.index, atlas.texture, atlas.width, atlas.height))
			{
				return std::unexpected("Invalid font atlas data");
			}
		}

		if (!reader.ReadCount(count, sizeof(FontCharacterData)))
		{
			return std::unexpected("Invalid font character count");
		}

		std::vector<FontCharacterData> parsedCharacters(count);
		for (auto& character : parsedCharacters)
		{
			if (!reader.Read(character.unicode, character.glyphIndex, character.atlasIndex,
				character.characterScale, character.glyphScale, character.glyphRect, character.sampleRect,
				character.uvMin, character.uvMax, character.width, character.height,
				character.bearingX, character.bearingY, character.advance))
			{
				return std::unexpected("Invalid font character data");
			}
		}

		if (!reader.IsAtEnd())
		{
			return std::unexpected("Unexpected trailing font data");
		}

		if (pointSize <= 0.0f || faceScale <= 0.0f || atlasPadding < 0 || parsedAtlases.empty())
		{
			return std::unexpected("Invalid font metrics or missing atlases");
		}

		std::unordered_map<i32, const FontAtlasData*> atlases;
		for (const auto& atlas : parsedAtlases)
		{
			if (atlas.index < 0 || atlas.width <= 0 || atlas.height <= 0 || !atlases.emplace(atlas.index, &atlas).second)
			{
				return std::unexpected("Invalid or duplicate font atlas");
			}
		}

		std::unordered_set<u32> characters;
		for (const auto& character : parsedCharacters)
		{
			const auto page = atlases.find(character.atlasIndex);
			if (page == atlases.end() || character.unicode > 0x10FFFF ||
				(character.unicode >= 0xD800 && character.unicode <= 0xDFFF) || !characters.insert(character.unicode).second)
			{
				return std::unexpected("Invalid font character mapping");
			}
			for (const auto& rect : { character.glyphRect, character.sampleRect })
			{
				if (rect[0] < 0 || rect[1] < 0 || rect[2] < 0 || rect[3] < 0 ||
					rect[0] + rect[2] > page->second->width || rect[1] + rect[3] > page->second->height)
				{
					return std::unexpected("Font glyph rectangle exceeds its atlas");
				}
			}
		}

		auto font = std::make_unique<BitmapFontAsset>(path);
		font->SetName(std::move(name));
		font->SetRenderMode(std::move(renderMode));
		font->SetDistanceField(isDistanceField);
		font->SetPointSize(pointSize);
		font->SetFaceScale(faceScale);
		font->SetLineHeight(lineHeight);
		font->SetAscentLine(ascentLine);
		font->SetDescentLine(descentLine);
		font->SetAtlasPadding(atlasPadding);
		font->SetAtlasData(std::move(parsedAtlases));
		font->SetCharacters(std::move(parsedCharacters));
		return font;
	}
}
