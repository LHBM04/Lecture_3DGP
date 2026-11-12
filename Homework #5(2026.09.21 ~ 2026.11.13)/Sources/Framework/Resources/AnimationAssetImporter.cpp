#include "Precompiled.hpp"
#include "AnimationAssetImporter.hpp"

#include "../Core/BinaryReader.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<AnimationAsset>, std::string> AnimationAssetImporter::Import(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".tukanim")
		{
			return std::unexpected("Animation requires a .tukanim file");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("Cannot open resource: " + path.generic_string());
		}

		BinaryReader reader(file);
		std::string name;
		f32 length = 0.0f;
		f32 frameRate = 0.0f;
		if (!reader.Read(name, length, frameRate))
		{
			return std::unexpected("Invalid animation header");
		}

		i32 count = 0;
		if (!reader.ReadCount(count, sizeof(AnimationTransform)))
		{
			return std::unexpected("Invalid animation transform count");
		}

		std::vector<AnimationTransform> transforms(count);
		for (auto& transform : transforms)
		{
			if (!reader.Read(transform.path) || !reader.ReadCount(count, sizeof(AnimationFrame)))
			{
				return std::unexpected("Invalid animation transform data");
			}
			transform.frames.resize(count);
			for (auto& frame : transform.frames)
			{
				if (!reader.Read(frame.time, frame.position, frame.rotation, frame.scale))
				{
					return std::unexpected("Invalid animation frame data");
				}
			}
		}

		if (!reader.IsAtEnd())
		{
			return std::unexpected("Unexpected trailing animation data");
		}

		if (length < 0.0f || frameRate <= 0.0f || transforms.empty())
		{
			return std::unexpected("Invalid animation timing or transform count");
		}

		std::unordered_set<std::string> paths;
		for (const auto& transform : transforms)
		{
			if (!paths.insert(transform.path).second || transform.frames.empty())
			{
				return std::unexpected("Duplicate animation target or missing frames");
			}

			f32 previous = -1.0f;
			for (const auto& frame : transform.frames)
			{
				f32 magnitude = 0.0f;
				for (f32 component : frame.rotation)
				{
					magnitude += component * component;
				}
				if (frame.time < 0.0f || frame.time < previous || frame.time > length ||
					!std::isfinite(magnitude) || magnitude < 0.000001f)
				{
					return std::unexpected("Invalid animation frame time or rotation");
				}
				previous = frame.time;
			}
		}

		return std::make_unique<AnimationAsset>(path, std::move(name), length, frameRate, std::move(transforms));
	}
}
