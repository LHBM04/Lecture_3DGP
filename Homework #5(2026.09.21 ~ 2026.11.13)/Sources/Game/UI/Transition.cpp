#include "Precompiled.hpp"
#include "Transition.hpp"

#include "../Core/Time.hpp"

namespace TUK::Game
{
	void Transition::OnPlayingUpdate()
	{
		BaseTransition::OnPlayingUpdate();

		const f32 rawDeltaTime = Time::GetUnscaledDeltaTime();
		if (!std::isfinite(rawDeltaTime) || rawDeltaTime < 0.0f)
		{
			return;
		}

		const f32 deltaTime = std::min(rawDeltaTime, MAX_DELTA_TIME);
		switch (GetState())
		{
			case TransitionState::OPEN:
			{
				if (openDuration <= 0.0f)
				{
					OpenComplete();
					break;
				}

				SetProgress(GetProgress() + (deltaTime / openDuration));
				if (GetProgress() >= 1.0f)
				{
					OpenComplete();
				}
				break;
			}
			case TransitionState::CLOSE:
			{
				if (closeDuration <= 0.0f)
				{
					CloseComplete();
					break;
				}

				SetProgress(GetProgress() - (deltaTime / closeDuration));
				if (GetProgress() <= 0.0f)
				{
					CloseComplete();
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}

	void Transition::Reset() noexcept
	{
		BaseTransition::Reset();
	}

	void Transition::Open()
	{
		if (GetState() != TransitionState::CLOSE)
		{
			SetProgress(0.0f);
		}

		BaseTransition::Open();
		onOpenBegin.Broadcast();
	}

	void Transition::OpenComplete()
	{
		if (GetState() == TransitionState::OPEN_COMPLETE)
		{
			return;
		}

		SetProgress(1.0f);
		onOpenComplete.Broadcast();
		BaseTransition::OpenComplete();
	}

	void Transition::Close()
	{
		if (GetState() == TransitionState::IDLE)
		{
			return;
		}

		if (GetState() != TransitionState::OPEN)
		{
			SetProgress(1.0f);
		}

		BaseTransition::Close();
		onCloseBegin.Broadcast();
	}

	void Transition::CloseComplete()
	{
		if (GetState() == TransitionState::CLOSE_COMPLETE)
		{
			return;
		}

		BaseTransition::CloseComplete();
		onCloseComplete.Broadcast();
	}

	void Transition::Play()
	{
		if (IsPlaying())
		{
			return;
		}

		BaseTransition::Play();
		onPlay.Broadcast();
	}

	void Transition::Pause()
	{
		if (!IsPlaying())
		{
			return;
		}

		BaseTransition::Pause();
		onPause.Broadcast();
	}

	void Transition::Stop()
	{
		if (GetState() == TransitionState::IDLE)
		{
			return;
		}

		BaseTransition::Stop();
		onStop.Broadcast();
	}

	void Transition::SetProgress(f32 value) noexcept
	{
		const f32 previous = GetProgress();
		BaseTransition::SetProgress(value);
		if (GetProgress() != previous)
		{
			onProgressChanged.Broadcast(GetProgress());
		}
	}
}
