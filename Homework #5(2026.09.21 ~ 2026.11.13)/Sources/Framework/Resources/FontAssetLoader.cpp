#include "Precompiled.hpp"
#include "FontAssetLoader.hpp"

#include <unordered_map>
#include <unordered_set>

#include "AssetSubsystem.hpp"
#include "ResourceImporter.hpp"
#include "TextureAsset.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<FontAsset>, std::string> FontAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".tukfont")
		{
			return std::unexpected("Font requires a .tukfont file");
		}
		auto data = ResourceImporter::Load<FontImportData>(path, ResourceType::FONT);
		if (!data)
		{
			return std::unexpected(data.error());
		}
		if (data->pointSize <= 0 || data->faceScale <= 0 || data->atlasPadding < 0 || data->atlases.empty())
		{
			return std::unexpected("Invalid font metrics or missing atlases");
		}

		std::unordered_map<i32, const FontAtlasImportData*> atlases;
		std::vector<const TextureAsset*> textures;
		for (const auto& atlas : data->atlases)
		{
			if (atlas.index < 0 || atlas.width <= 0 || atlas.height <= 0 || !atlases.emplace(atlas.index, &atlas).second)
			{
				return std::unexpected("Invalid or duplicate font atlas");
			}
			const auto texturePath = ResourceMetadata::ResolvePath(path.parent_path(), atlas.texture);
			const TextureAsset* texture = texturePath ? GetOwner()->LoadAsset<TextureAsset>(*texturePath) : nullptr;
			if (!texture)
			{
				return std::unexpected("Cannot load font atlas: " + atlas.texture);
			}
			const auto description = texture->GetTexture().GetD3DResource().GetDesc();
			if (description.Width != static_cast<u64>(atlas.width) || description.Height != static_cast<u32>(atlas.height))
			{
				return std::unexpected("Font atlas dimensions do not match the metadata");
			}
			textures.push_back(texture);
		}

		std::unordered_set<u32> characters;
		for (const auto& character : data->characters)
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
		return std::make_unique<FontAsset>(path, std::move(*data), std::move(textures));
	}
}
