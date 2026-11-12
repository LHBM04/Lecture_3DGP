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
			return std::unexpected("애니메이션 로더에는 .tukanim 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("리소스를 열 수 없습니다: " + path.generic_string());
		}

		BinaryReader reader(file);
		std::string name;
		f32 length = 0.0f;
		f32 frameRate = 0.0f;
		if (!reader.Read(name, length, frameRate))
		{
			return std::unexpected("애니메이션 헤더가 유효하지 않습니다.");
		}

		i32 count = 0;
		if (!reader.ReadCount(count, sizeof(AnimationTransform)))
		{
			return std::unexpected("애니메이션 트랜스폼 개수가 유효하지 않습니다.");
		}

		std::vector<AnimationTransform> transforms(count);
		for (auto& transform : transforms)
		{
			if (!reader.Read(transform.path) || !reader.ReadCount(count, sizeof(AnimationFrame)))
			{
				return std::unexpected("애니메이션 트랜스폼 데이터가 유효하지 않습니다.");
			}
			transform.frames.resize(count);
			for (auto& frame : transform.frames)
			{
				if (!reader.Read(frame.time, frame.position, frame.rotation, frame.scale))
				{
					return std::unexpected("애니메이션 프레임 데이터가 유효하지 않습니다.");
				}
			}
		}

		if (!reader.IsAtEnd())
		{
			return std::unexpected("애니메이션 파일 끝에 예상치 못한 데이터가 남아 있습니다.");
		}

		if (length < 0.0f || frameRate <= 0.0f || transforms.empty())
		{
			return std::unexpected("애니메이션 시간 정보 또는 트랜스폼 개수가 유효하지 않습니다.");
		}

		std::unordered_set<std::string> paths;
		for (const auto& transform : transforms)
		{
			if (!paths.insert(transform.path).second || transform.frames.empty())
			{
				return std::unexpected("중복된 애니메이션 대상이 있거나 프레임이 비어 있습니다.");
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
					return std::unexpected("애니메이션 프레임 시간 또는 회전값이 유효하지 않습니다.");
				}
				previous = frame.time;
			}
		}

		return std::make_unique<AnimationAsset>(path, std::move(name), length, frameRate, std::move(transforms));
	}
}
