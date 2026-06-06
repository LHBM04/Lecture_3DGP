#pragma once

#include <fstream>
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
	bool ReadTag(std::ifstream& file_, const std::string& expectedTag_);
	std::string ReadTag(std::ifstream& file_);
	std::wstring ReadString(std::ifstream& file_);

	template <typename T>
	bool ReadValue(std::ifstream& file_, T& value_)
	{
		return static_cast<bool>(file_.read(reinterpret_cast<char*>(&value_), sizeof(T)));
	}

	std::wstring animationName;
	float duration{ 0.0f };
	float ticksPerSecond{ 30.0f };
	std::vector<AnimationTrack> tracks;
};
