#pragma once

#include <string>
#include <vector>

#include "Quaternion.h"
#include "Resource.h"
#include "Vector3D.h"

struct KeyframeData final
{
	float time{ 0.0f };
	Vector3D position;
	Quaternion rotation;
	Vector3D scale;
};

struct AnimationTrack final
{
	std::wstring nodeName;
	std::vector<KeyframeData> keyframes;
};

class AnimationClip final : public Resource
{
public:
	AnimationClip() = default;
	~AnimationClip() override = default;

	bool Load() override;
	void Unload() override;

	[[nodiscard]] const std::wstring& GetAnimationName() const noexcept;
	[[nodiscard]] float GetDuration() const noexcept;
	[[nodiscard]] float GetTicksPerSecond() const noexcept;
	[[nodiscard]] const std::vector<AnimationTrack>& GetTracks() const noexcept;

private:
	std::wstring animationName;
	float duration{ 0.0f };
	float ticksPerSecond{ 30.0f };
	std::vector<AnimationTrack> tracks;
};
