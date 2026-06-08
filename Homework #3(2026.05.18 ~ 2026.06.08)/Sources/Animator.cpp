#include "Precompiled.h"
#include "Animator.h"

#include "AnimationClip.h"
#include "GameObject.h"
#include "MathF.h"
#include "ResourceSystem.h"
#include "TimeSystem.h"
#include "Transform.h"

void Animator::Play(AnimationClip* clip_, bool loop_)
{
	if (currentClip == clip_ && isPlaying)
	{
		return;
	}

	currentClip = clip_;
	isLooping = loop_;
	currentTime = 0.0f;
	isPlaying = (currentClip != nullptr);
}

void Animator::Stop()
{
	isPlaying = false;
	currentTime = 0.0f;
}

void Animator::Pause()
{
	isPlaying = false;
}

void Animator::Resume()
{
	if (currentClip != nullptr)
	{
		isPlaying = true;
	}
}

bool Animator::IsPlaying() const noexcept
{
	return isPlaying;
}

AnimationClip* Animator::GetCurrentClip() const noexcept
{
	return currentClip;
}

void Animator::OnAwake()
{
	RefreshNodeMap();
}

void Animator::OnUpdate()
{
	if (!isPlaying || currentClip == nullptr)
	{
		return;
	}

	currentTime += TimeSystem::GetInstance().GetDeltaTime();

	float duration{ currentClip->GetDuration() };
	if (currentTime >= duration)
	{
		if (isLooping)
		{
			currentTime = std::fmod(currentTime, duration);
		}
		else
		{
			currentTime = duration;
			isPlaying = false;
		}
	}

	ApplyAnimation(currentTime);
}

void Animator::OnDisable()
{
	Stop();
}

void Animator::RefreshNodeMap()
{
	nodeMap.clear();

	GameObject* const owner{ GetOwner() };
	Transform* const rootTransform{ owner->GetComponent<Transform>() };

	auto traverse = [&](this auto& self, Transform* current_) -> void
	{
		if (current_ == nullptr)
		{
			return;
		}

		this->nodeMap[current_->GetOwner()->GetName()] = current_;

		for (Transform* child : current_->GetChildren())
		{
			self(child);
		}
	};

	traverse(rootTransform);
}

void Animator::ApplyAnimation(float time_)
{
	if (currentClip == nullptr)
	{
		return;
	}

	for (const AnimationTrack& track : currentClip->GetTracks())
	{
		if (track.keyframes.empty())
		{
			continue;
		}

		Transform* targetTransform{ nullptr };
		
		if (track.nodeName == L"Self")
		{
			targetTransform = GetOwner()->GetComponent<Transform>();
		}
		else
		{
			auto it{ nodeMap.find(track.nodeName) };
			if (it != nodeMap.end())
			{
				targetTransform = it->second;
			}
		}

		if (targetTransform == nullptr)
		{
			continue;
		}

		if (HasPositionChanged(track.keyframes))
		{
			targetTransform->SetLocalPosition(InterpolatePosition(track.keyframes, time_));
		}
		
		if (HasScaleChanged(track.keyframes))
		{
			targetTransform->SetLocalScale(InterpolateScale(track.keyframes, time_));
		}

		if (HasRotationChanged(track.keyframes))
		{
			targetTransform->SetLocalRotation(InterpolateRotation(track.keyframes, time_));
		}
	}
}

bool Animator::HasPositionChanged(const std::vector<KeyframeData>& keys_) const
{
	if (keys_.size() <= 1)
	{
		return false;
	}

	const Vector3D& first{ keys_.front().position };
	for (std::size_t i{ 1 }; i < keys_.size(); ++i)
	{
		if (Vector3D::Distance(first, keys_[i].position) > 0.001f)
		{
			return true;
		}
	}

	return false;
}

bool Animator::HasRotationChanged(const std::vector<KeyframeData>& keys_) const
{
	if (keys_.size() <= 1)
	{
		return false;
	}

	const Quaternion& first{ keys_.front().rotation };
	for (std::size_t i{ 1 }; i < keys_.size(); ++i)
	{
		if (Quaternion::Angle(first, keys_[i].rotation) > 0.001f)
		{
			return true;
		}
	}

	return false;
}

bool Animator::HasScaleChanged(const std::vector<KeyframeData>& keys_) const
{
	if (keys_.size() <= 1)
	{
		return false;
	}

	const Vector3D& first{ keys_.front().scale };
	for (std::size_t i{ 1 }; i < keys_.size(); ++i)
	{
		if (Vector3D::Distance(first, keys_[i].scale) > 0.001f)
		{
			return true;
		}
	}

	return false;
}

Vector3D Animator::InterpolatePosition(const std::vector<KeyframeData>& keys_, float time_) const
{
	if (keys_.empty())
	{
		return Vector3D::GetZero();
	}

	if (keys_.size() == 1 || time_ <= keys_.front().time)
	{
		return keys_.front().position;
	}

	if (time_ >= keys_.back().time)
	{
		return keys_.back().position;
	}

	for (std::size_t i{ 0 }; i < keys_.size() - 1; ++i)
	{
		const KeyframeData& curr{ keys_[i] };
		const KeyframeData& next{ keys_[i + 1] };

		if (time_ >= curr.time && time_ < next.time)
		{
			const float t{ (time_ - curr.time) / (next.time - curr.time) };
			return Vector3D::Lerp(curr.position, next.position, t);
		}
	}

	return keys_.back().position;
}

Quaternion Animator::InterpolateRotation(const std::vector<KeyframeData>& keys_, float time_) const
{
	if (keys_.empty())
	{
		return Quaternion::GetIdentity();
	}

	if (keys_.size() == 1 || time_ <= keys_.front().time)
	{
		return keys_.front().rotation;
	}

	if (time_ >= keys_.back().time)
	{
		return keys_.back().rotation;
	}

	for (std::size_t i{ 0 }; i < keys_.size() - 1; ++i)
	{
		const KeyframeData& curr{ keys_[i] };
		const KeyframeData& next{ keys_[i + 1] };

		if (time_ >= curr.time && time_ < next.time)
		{
			const float t{ (time_ - curr.time) / (next.time - curr.time) };
			return Quaternion::Slerp(curr.rotation, next.rotation, t);
		}
	}

	return keys_.back().rotation;
}

Vector3D Animator::InterpolateScale(const std::vector<KeyframeData>& keys_, float time_) const
{
	if (keys_.empty())
	{
		return Vector3D::GetOne();
	}

	if (keys_.size() == 1 || time_ <= keys_.front().time)
	{
		return keys_.front().scale;
	}

	if (time_ >= keys_.back().time)
	{
		return keys_.back().scale;
	}

	for (std::size_t i{ 0 }; i < keys_.size() - 1; ++i)
	{
		const KeyframeData& curr{ keys_[i] };
		const KeyframeData& next{ keys_[i + 1] };

		if (time_ >= curr.time && time_ < next.time)
		{
			const float t{ (time_ - curr.time) / (next.time - curr.time) };
			return Vector3D::Lerp(curr.scale, next.scale, t);
		}
	}

	return keys_.back().scale;
}
