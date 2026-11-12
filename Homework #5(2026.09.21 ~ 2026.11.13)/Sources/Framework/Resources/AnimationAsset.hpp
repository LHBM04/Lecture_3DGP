#pragma once

#include "../Core/Common.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	struct AnimationFrame final
	{
		f32 time = 0.0f;
		std::array<f32, 3> position{};
		std::array<f32, 4> rotation{};
		std::array<f32, 3> scale{};
	};

	struct AnimationTransform final
	{
		std::string path;
		std::vector<AnimationFrame> frames;
	};

	class AnimationAsset final : public Asset
	{
	public:
		explicit AnimationAsset(
			std::filesystem::path path,
			std::string name,
			f32 length,
			f32 frameRate,
			std::vector<AnimationTransform>&& transforms) noexcept;

		[[nodiscard]] const std::string& GetName() const noexcept;
		[[nodiscard]] f32 GetLength() const noexcept;
		[[nodiscard]] f32 GetFrameRate() const noexcept;
		[[nodiscard]] std::span<const AnimationTransform> GetTransforms() const noexcept;

	private:
		std::string name;
		f32 length = 0.0f;
		f32 frameRate = 0.0f;
		std::vector<AnimationTransform> transforms;
	};

	inline AnimationAsset::AnimationAsset(
		std::filesystem::path path,
		std::string name,
		f32 length,
		f32 frameRate,
		std::vector<AnimationTransform>&& transforms) noexcept
		: Asset(std::move(path))
		, name(std::move(name))
		, length(length)
		, frameRate(frameRate)
		, transforms(std::move(transforms))
	{
	}

	inline const std::string& AnimationAsset::GetName() const noexcept
	{
		return name;
	}

	inline f32 AnimationAsset::GetLength() const noexcept
	{
		return length;
	}

	inline f32 AnimationAsset::GetFrameRate() const noexcept
	{
		return frameRate;
	}

	inline std::span<const AnimationTransform> AnimationAsset::GetTransforms() const noexcept
	{
		return transforms;
	}
}
