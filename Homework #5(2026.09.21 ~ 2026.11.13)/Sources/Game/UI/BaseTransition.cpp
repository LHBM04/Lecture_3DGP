#include "Precompiled.hpp"
#include "BaseTransition.hpp"

namespace TUK::Game
{
	void BaseTransition::OnAwake()
	{
		Reset();
	}

	void BaseTransition::OnUpdate()
	{
		if (isPlaying)
		{
			OnPlayingUpdate();
		}
	}

	void BaseTransition::OnPlayingUpdate()
	{
	}

	void BaseTransition::Reset() noexcept
	{
		isPlaying = false;
		state = TransitionState::IDLE;
		progress = 0.0f;
	}

	void BaseTransition::Open()
	{
		if (GameObject* owner = GetOwner())
		{
			owner->SetEnabled(true);
		}

		state = TransitionState::OPEN;
		Play();
	}

	void BaseTransition::OpenComplete()
	{
		if (state == TransitionState::OPEN_COMPLETE)
		{
			return;
		}

		SetProgress(1.0f);
		if (closeAfterOpen)
		{
			Close();
		}
		else
		{
			Pause();
			state = TransitionState::OPEN_COMPLETE;
		}
	}

	void BaseTransition::Close()
	{
		if (state == TransitionState::IDLE)
		{
			return;
		}

		state = TransitionState::CLOSE;
		Play();
	}

	void BaseTransition::CloseComplete()
	{
		if (state == TransitionState::CLOSE_COMPLETE)
		{
			return;
		}

		SetProgress(0.0f);
		Pause();
		state = TransitionState::CLOSE_COMPLETE;
	}

	void BaseTransition::Play()
	{
		if (isPlaying)
		{
			return;
		}

		isPlaying = true;
		if (state == TransitionState::IDLE || state == TransitionState::CLOSE_COMPLETE)
		{
			Open();
		}
		else if (state == TransitionState::OPEN_COMPLETE)
		{
			Close();
		}
	}

	void BaseTransition::Pause()
	{
		if (!isPlaying)
		{
			return;
		}

		isPlaying = false;
	}

	void BaseTransition::Stop()
	{
		if (state == TransitionState::IDLE)
		{
			return;
		}

		Pause();
		state = TransitionState::IDLE;
		SetProgress(0.0f);
	}
}
