#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Component.h"
#include "Quaternion.h"
#include "Vector3D.h"

class AnimationClip;
class Transform;
struct KeyframeData;

class Animator final : public Component
{
public:
	Animator() = default;
	~Animator() override = default;

	void Play(AnimationClip* clip_, bool loop_ = true);
	void Stop();
	void Pause();
	void Resume();

	[[nodiscard]] bool IsPlaying() const noexcept;
	[[nodiscard]] AnimationClip* GetCurrentClip() const noexcept;

protected:
	void OnAwake() override;
	void OnUpdate() override;
	void OnDisable() override;

private:
	void RefreshNodeMap();
	void ApplyAnimation(float time_);

	[[nodiscard]] bool HasPositionChanged(const std::vector<KeyframeData>& keys_) const;
	[[nodiscard]] bool HasRotationChanged(const std::vector<KeyframeData>& keys_) const;
	[[nodiscard]] bool HasScaleChanged(const std::vector<KeyframeData>& keys_) const;

	[[nodiscard]] Vector3D InterpolatePosition(const std::vector<KeyframeData>& keys_, float time_) const;
	[[nodiscard]] Quaternion InterpolateRotation(const std::vector<KeyframeData>& keys_, float time_) const;
	[[nodiscard]] Vector3D InterpolateScale(const std::vector<KeyframeData>& keys_, float time_) const;

	AnimationClip* currentClip{ nullptr };
	float currentTime{ 0.0f };
	bool isPlaying{ false };
	bool isLooping{ true };

	std::unordered_map<std::wstring, Transform*> nodeMap;
};
