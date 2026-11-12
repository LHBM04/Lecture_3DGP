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
			return std::unexpected("폰트 로더에는 .tukfont 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("리소스를 열 수 없습니다: " + path.generic_string());
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
			return std::unexpected("폰트 헤더가 유효하지 않습니다.");
		}

		i32 count = 0;
		if (!reader.ReadCount(count, sizeof(FontAtlasData)))
		{
			return std::unexpected("폰트 아틀라스 개수가 유효하지 않습니다.");
		}

		std::vector<FontAtlasData> parsedAtlases(count);
		for (auto& atlas : parsedAtlases)
		{
			if (!reader.Read(atlas.index, atlas.texture, atlas.width, atlas.height))
			{
				return std::unexpected("폰트 아틀라스 데이터가 유효하지 않습니다.");
			}
		}

		if (!reader.ReadCount(count, sizeof(FontCharacterData)))
		{
			return std::unexpected("폰트 문자 개수가 유효하지 않습니다.");
		}

		std::vector<FontCharacterData> parsedCharacters(count);
		for (auto& character : parsedCharacters)
		{
			if (!reader.Read(character.unicode, character.glyphIndex, character.atlasIndex,
				character.characterScale, character.glyphScale, character.glyphRect, character.sampleRect,
				character.uvMin, character.uvMax, character.width, character.height,
				character.bearingX, character.bearingY, character.advance))
			{
				return std::unexpected("폰트 문자 데이터가 유효하지 않습니다.");
			}
		}

		if (!reader.IsAtEnd())
		{
			return std::unexpected("폰트 파일 끝에 예상치 못한 데이터가 남아 있습니다.");
		}

		if (pointSize <= 0.0f || faceScale <= 0.0f || atlasPadding < 0 || parsedAtlases.empty())
		{
			return std::unexpected("폰트 메트릭이 유효하지 않거나 아틀라스가 없습니다.");
		}

		std::unordered_map<i32, const FontAtlasData*> atlases;
		for (const auto& atlas : parsedAtlases)
		{
			if (atlas.index < 0 || atlas.width <= 0 || atlas.height <= 0 || !atlases.emplace(atlas.index, &atlas).second)
			{
				return std::unexpected("폰트 아틀라스가 유효하지 않거나 중복되었습니다.");
			}
		}

		std::unordered_set<u32> characters;
		for (const auto& character : parsedCharacters)
		{
			const auto page = atlases.find(character.atlasIndex);
			if (page == atlases.end() || character.unicode > 0x10FFFF ||
				(character.unicode >= 0xD800 && character.unicode <= 0xDFFF) || !characters.insert(character.unicode).second)
			{
				return std::unexpected("폰트 문자 매핑이 유효하지 않습니다.");
			}
			for (const auto& rect : { character.glyphRect, character.sampleRect })
			{
				if (rect[0] < 0 || rect[1] < 0 || rect[2] < 0 || rect[3] < 0 ||
					rect[0] + rect[2] > page->second->width || rect[1] + rect[3] > page->second->height)
				{
					return std::unexpected("폰트 글리프 영역이 아틀라스 범위를 벗어났습니다.");
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
