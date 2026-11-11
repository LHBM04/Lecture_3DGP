#pragma once

#include <array>
#include <string>
#include <vector>

#include "../Core/BinaryReader.hpp"

namespace TUK::Framework
{
	struct FontAtlasImportData final
	{
		i32 index{};
		std::string texture{};
		i32 width{};
		i32 height{};

		[[nodiscard]] bool Deserialize(BinaryReader& reader);
	};

	struct FontCharacterImportData final
	{
		u32 unicode{};
		u32 glyphIndex{};
		i32 atlasIndex{};
		f32 characterScale{};
		f32 glyphScale{};
		std::array<f32, 4> glyphRect{};
		std::array<f32, 4> sampleRect{};
		std::array<f32, 2> uvMin{};
		std::array<f32, 2> uvMax{};
		f32 width{};
		f32 height{};
		f32 bearingX{};
		f32 bearingY{};
		f32 advance{};

		[[nodiscard]] bool Deserialize(BinaryReader& reader);
	};

	struct FontImportData final
	{
		std::string name{};
		std::string renderMode{};
		bool isDistanceField{};
		f32 pointSize{};
		f32 faceScale{};
		f32 lineHeight{};
		f32 ascentLine{};
		f32 descentLine{};
		i32 atlasPadding{};
		std::vector<FontAtlasImportData> atlases{};
		std::vector<FontCharacterImportData> characters{};

		[[nodiscard]] bool Deserialize(BinaryReader& reader);
	};

	struct AnimationFrameMetadata final
	{
		f32 time{};
		std::array<f32, 3> position{};
		std::array<f32, 4> rotation{};
		std::array<f32, 3> scale{};

		[[nodiscard]] bool Deserialize(BinaryReader& reader);
	};

	struct AnimationTransformMetadata final
	{
		std::string path{};
		std::vector<AnimationFrameMetadata> frames{};

		[[nodiscard]] bool Deserialize(BinaryReader& reader);
	};

	struct AnimationMetadata final
	{
		std::string name{};
		f32 length{};
		f32 frameRate{};
		std::vector<AnimationTransformMetadata> transforms{};

		[[nodiscard]] bool Deserialize(BinaryReader& reader);
	};

	inline bool FontAtlasImportData::Deserialize(BinaryReader& reader)
	{
		return reader.Read(index, texture, width, height);
	}

	inline bool FontCharacterImportData::Deserialize(BinaryReader& reader)
	{
		return reader.Read(
			unicode,
			glyphIndex,
			atlasIndex,
			characterScale,
			glyphScale,
			glyphRect,
			sampleRect,
			uvMin,
			uvMax,
			width,
			height,
			bearingX,
			bearingY,
			advance);
	}

	inline bool FontImportData::Deserialize(BinaryReader& reader)
	{
		return reader.Read(
			name,
			renderMode,
			isDistanceField,
			pointSize,
			faceScale,
			lineHeight,
			ascentLine,
			descentLine,
			atlasPadding,
			atlases,
			characters);
	}

	inline bool AnimationFrameMetadata::Deserialize(BinaryReader& reader)
	{
		return reader.Read(time, position, rotation, scale);
	}

	inline bool AnimationTransformMetadata::Deserialize(BinaryReader& reader)
	{
		return reader.Read(path, frames);
	}

	inline bool AnimationMetadata::Deserialize(BinaryReader& reader)
	{
		return reader.Read(name, length, frameRate, transforms);
	}

}
