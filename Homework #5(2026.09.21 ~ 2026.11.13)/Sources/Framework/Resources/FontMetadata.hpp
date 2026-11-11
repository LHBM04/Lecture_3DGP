#pragma once

#include <array>
#include <string>
#include <vector>

#include "ResourceMetadata.hpp"

namespace TUK::Framework
{
	// FontMetadata::format: 1 = BMFont, 2 = TextMesh Pro. Page paths are relative to the font file.
	struct BitmapFontInfoMetadata final
	{
		std::string face{};
		i32 size{};
		i32 bold{};
		i32 italic{};
		std::string charset{};
		i32 unicode{};
		i32 stretchH{};
		i32 smooth{};
		i32 aa{};
		std::array<i32, 4> padding{};
		std::array<i32, 2> spacing{};
		i32 outline{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct BitmapFontCommonMetadata final
	{
		i32 lineHeight{};
		i32 base{};
		i32 scaleW{};
		i32 scaleH{};
		i32 pages{};
		i32 packed{};
		i32 alphaChnl{};
		i32 redChnl{};
		i32 greenChnl{};
		i32 blueChnl{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct BitmapFontPageMetadata final
	{
		i32 id{};
		std::string file{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct BitmapFontCharacterMetadata final
	{
		i32 id{};
		i32 x{};
		i32 y{};
		i32 width{};
		i32 height{};
		i32 xoffset{};
		i32 yoffset{};
		i32 xadvance{};
		i32 page{};
		i32 chnl{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct FontKerningMetadata final
	{
		i32 first{};
		i32 second{};
		i32 amount{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct BitmapFontMetadata final
	{
		BitmapFontInfoMetadata info{};
		BitmapFontCommonMetadata common{};
		std::vector<BitmapFontPageMetadata> pages{};
		std::vector<BitmapFontCharacterMetadata> chars{};
		std::vector<FontKerningMetadata> kernings{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct FontRectMetadata final
	{
		i32 x{};
		i32 y{};
		i32 width{};
		i32 height{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct TMPFontAtlasMetadata final
	{
		i32 index{};
		std::string texture{};
		i32 width{};
		i32 height{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct TMPFontCharacterMetadata final
	{
		u32 unicode{};
		u32 glyphIndex{};
		i32 atlasIndex{};
		f32 characterScale{};
		f32 glyphScale{};
		FontRectMetadata glyphRect{};
		FontRectMetadata sampleRect{};
		std::array<f32, 2> uvMin{};
		std::array<f32, 2> uvMax{};
		f32 width{};
		f32 height{};
		f32 bearingX{};
		f32 bearingY{};
		f32 advance{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct TMPFontMetadata final
	{
		u32 version{};
		std::string name{};
		std::string renderMode{};
		u32 isDistanceField{};
		std::string maskChannel{};
		std::string textureColorSpace{};
		std::string coordinateOrigin{};
		f32 pointSize{};
		f32 faceScale{};
		f32 lineHeight{};
		f32 ascentLine{};
		f32 descentLine{};
		i32 atlasPadding{};
		std::vector<TMPFontAtlasMetadata> atlases{};
		std::vector<TMPFontCharacterMetadata> characters{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct FontMetadata final
	{
		u32 format{};
		BitmapFontMetadata bitmap{};
		TMPFontMetadata tmp{};
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	template <class TArchive>
	bool BitmapFontInfoMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, face)
			&& archive.Field(2, size)
			&& archive.Field(3, bold)
			&& archive.Field(4, italic)
			&& archive.Field(5, charset)
			&& archive.Field(6, unicode)
			&& archive.Field(7, stretchH)
			&& archive.Field(8, smooth)
			&& archive.Field(9, aa)
			&& archive.Field(10, padding)
			&& archive.Field(11, spacing)
			&& archive.Field(12, outline);
	}

	template <class TArchive>
	bool BitmapFontCommonMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, lineHeight)
			&& archive.Field(2, base)
			&& archive.Field(3, scaleW)
			&& archive.Field(4, scaleH)
			&& archive.Field(5, pages)
			&& archive.Field(6, packed)
			&& archive.Field(7, alphaChnl)
			&& archive.Field(8, redChnl)
			&& archive.Field(9, greenChnl)
			&& archive.Field(10, blueChnl);
	}

	template <class TArchive>
	bool BitmapFontPageMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, id)
			&& archive.Field(2, file);
	}

	template <class TArchive>
	bool BitmapFontCharacterMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, id)
			&& archive.Field(2, x)
			&& archive.Field(3, y)
			&& archive.Field(4, width)
			&& archive.Field(5, height)
			&& archive.Field(6, xoffset)
			&& archive.Field(7, yoffset)
			&& archive.Field(8, xadvance)
			&& archive.Field(9, page)
			&& archive.Field(10, chnl);
	}

	template <class TArchive>
	bool FontKerningMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, first)
			&& archive.Field(2, second)
			&& archive.Field(3, amount);
	}

	template <class TArchive>
	bool BitmapFontMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, info)
			&& archive.Field(2, common)
			&& archive.Field(3, pages)
			&& archive.Field(4, chars)
			&& archive.Field(5, kernings);
	}

	template <class TArchive>
	bool FontRectMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, x)
			&& archive.Field(2, y)
			&& archive.Field(3, width)
			&& archive.Field(4, height);
	}

	template <class TArchive>
	bool TMPFontAtlasMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, index)
			&& archive.Field(2, texture)
			&& archive.Field(3, width)
			&& archive.Field(4, height);
	}

	template <class TArchive>
	bool TMPFontCharacterMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, unicode)
			&& archive.Field(2, glyphIndex)
			&& archive.Field(3, atlasIndex)
			&& archive.Field(4, characterScale)
			&& archive.Field(5, glyphScale)
			&& archive.Field(6, glyphRect)
			&& archive.Field(7, sampleRect)
			&& archive.Field(8, uvMin)
			&& archive.Field(9, uvMax)
			&& archive.Field(10, width)
			&& archive.Field(11, height)
			&& archive.Field(12, bearingX)
			&& archive.Field(13, bearingY)
			&& archive.Field(14, advance);
	}

	template <class TArchive>
	bool TMPFontMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, version)
			&& archive.Field(2, name)
			&& archive.Field(3, renderMode)
			&& archive.Field(4, isDistanceField)
			&& archive.Field(5, maskChannel)
			&& archive.Field(6, textureColorSpace)
			&& archive.Field(7, coordinateOrigin)
			&& archive.Field(8, pointSize)
			&& archive.Field(9, faceScale)
			&& archive.Field(10, lineHeight)
			&& archive.Field(11, ascentLine)
			&& archive.Field(12, descentLine)
			&& archive.Field(13, atlasPadding)
			&& archive.Field(14, atlases)
			&& archive.Field(15, characters);
	}

	template <class TArchive>
	bool FontMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, format)
			&& archive.OptionalField(2, bitmap)
			&& archive.OptionalField(3, tmp);
	}

}
