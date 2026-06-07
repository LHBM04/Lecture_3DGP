#include "Precompiled.h"
#include "AnimationClip.h"

#include "Logger.h"

bool AnimationClip::Load()
{
	if (path.empty())
	{
		return false;
	}

	std::ifstream file{ path, std::ios::binary };
	if (!file.is_open())
	{
		Logger::Critical(L"[Animation] Load failed: cannot open file. path={}", path);
		return false;
	}

	if (!HasTag(file, "<Animation>:"))
	{
		Logger::Critical(L"[Animation] Load failed: <Animation>: tag not found. path={}", path);
		return false;
	}

	animationName = ReadString(file);
	
	if (!ReadValue(file, duration) || !ReadValue(file, ticksPerSecond))
	{
		Logger::Critical(L"[Animation] Load failed: cannot read duration/ticks. path={}", path);
		return false;
	}

	if (!HasTag(file, "<Tracks>:"))
	{
		Logger::Critical(L"[Animation] Load failed: <Tracks>: tag not found. path={}", path);
		return false;
	}

	uint32_t trackCount{ 0 };
	if (!ReadValue(file, trackCount))
	{
		Logger::Critical(L"[Animation] Load failed: cannot read track count. path={}", path);
		return false;
	}

	tracks.clear();
	tracks.reserve(trackCount);
	for (uint32_t i{ 0 }; i < trackCount; ++i)
	{
		if (!HasTag(file, "<Track>:"))
		{
			Logger::Critical(L"[Animation] Load failed: <Track>: tag not found. path={}", path);
			return false;
		}

		AnimationTrack track{};
		track.nodeName = ReadString(file);

		if (!HasTag(file, "<Keyframes>:"))
		{
			Logger::Critical(L"[Animation] Load failed: <Keyframes>: tag not found. path={}, node={}", path, track.nodeName);
			return false;
		}

		uint32_t keyCount{ 0 };
		if (!ReadValue(file, keyCount))
		{
			Logger::Critical(L"[Animation] Load failed: cannot read key count. path={}, node={}", path, track.nodeName);
			return false;
		}

		track.keyframes.reserve(keyCount);
		for (uint32_t k{ 0 }; k < keyCount; ++k)
		{
			KeyframeData key{};
			if (!ReadValue(file, key.time) || 
				!ReadValue(file, key.position) || 
				!ReadValue(file, key.rotation) || 
				!ReadValue(file, key.scale))
			{
				Logger::Critical(L"[Animation] Load failed: cannot read keyframe data. path={}, node={}", path, track.nodeName);
				return false;
			}
			track.keyframes.emplace_back(key);
		}

		tracks.emplace_back(std::move(track));
	}

	Logger::Trace(L"[Animation] 로드 완료. 경로={}, 이름={}, 트랙수={}, 시간={}", path, animationName, tracks.size(), duration);
	return true;
}

void AnimationClip::Unload()
{
	tracks.clear();
	animationName.clear();
	duration = 0.0f;
}

const std::wstring& AnimationClip::GetAnimationName() const noexcept
{
	return animationName;
}

float AnimationClip::GetDuration() const noexcept
{
	return duration;
}

float AnimationClip::GetTicksPerSecond() const noexcept
{
	return ticksPerSecond;
}

const std::vector<AnimationTrack>& AnimationClip::GetTracks() const noexcept
{
	return tracks;
}
