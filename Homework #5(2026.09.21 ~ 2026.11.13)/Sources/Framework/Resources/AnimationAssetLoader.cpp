#include "Precompiled.hpp"
#include "AnimationAssetLoader.hpp"

#include <cmath>
#include <unordered_set>

#include "ResourceImporter.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<AnimationAsset>, std::string> AnimationAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".tukanim")
		{
			return std::unexpected("Animation requires a .tukanim file");
		}
		auto data = ResourceImporter::Load<AnimationMetadata>(path, ResourceType::ANIMATION);
		if (!data)
		{
			return std::unexpected(data.error());
		}
		if (data->length < 0 || data->frameRate <= 0 || data->transforms.empty())
		{
			return std::unexpected("Invalid animation timing or transform count");
		}

		std::unordered_set<std::string> paths;
		for (const auto& transform : data->transforms)
		{
			if (!paths.insert(transform.path).second || transform.frames.empty())
			{
				return std::unexpected("Duplicate animation target or missing frames");
			}
			f32 previous = -1;
			for (const auto& frame : transform.frames)
			{
				f32 magnitude = 0;
				for (f32 component : frame.rotation)
				{
					magnitude += component * component;
				}
				if (frame.time < 0 || frame.time < previous || frame.time > data->length ||
					!std::isfinite(magnitude) || magnitude < 0.000001f)
				{
					return std::unexpected("Invalid animation frame time or rotation");
				}
				previous = frame.time;
			}
		}
		return std::make_unique<AnimationAsset>(path, std::move(*data));
	}
}
