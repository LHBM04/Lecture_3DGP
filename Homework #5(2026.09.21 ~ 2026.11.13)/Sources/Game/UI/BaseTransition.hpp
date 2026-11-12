#pragma once

#include "Framework/Core.hpp"
#include "Framework/Objects.hpp"

namespace TUK::Game
{
	using namespace Framework;

	enum class TransitionState : u8
	{
		IDLE,
		OPEN,
		OPEN_COMPLETE,
		CLOSE,
		CLOSE_COMPLETE
	};

	class BaseTransition : public Component
	{
	public:
		using Component::Component;

		void OnAwake() override;
		void OnUpdate() override;
		virtual void OnPlayingUpdate();

		virtual void Reset() noexcept;

		virtual void Open();
		virtual void OpenComplete();
		virtual void Close();
		virtual void CloseComplete();

		virtual void Play();
		virtual void Pause();
		virtual void Stop();

		[[nodiscard]] bool IsCloseAfterOpen() const noexcept;
		void SetCloseAfterOpen(bool value) noexcept;

		[[nodiscard]] bool IsPlaying() const noexcept;
		[[nodiscard]] TransitionState GetState() const noexcept;

		[[nodiscard]] f32 GetProgress() const noexcept;
		virtual void SetProgress(f32 value) noexcept;

	protected:
		void SetState(TransitionState value) noexcept;

	private:
		bool closeAfterOpen = false;
		bool isPlaying = false;
		TransitionState state = TransitionState::IDLE;
		f32 progress = 0.0f;
	};

	inline bool BaseTransition::IsCloseAfterOpen() const noexcept
	{
		return closeAfterOpen;
	}

	inline void BaseTransition::SetCloseAfterOpen(bool value) noexcept
	{
		closeAfterOpen = value;
	}

	inline bool BaseTransition::IsPlaying() const noexcept
	{
		return isPlaying;
	}

	inline TransitionState BaseTransition::GetState() const noexcept
	{
		return state;
	}

	inline f32 BaseTransition::GetProgress() const noexcept
	{
		return progress;
	}

	inline void BaseTransition::SetProgress(f32 value) noexcept
	{
		progress = std::clamp(value, 0.0f, 1.0f);
	}

	inline void BaseTransition::SetState(TransitionState value) noexcept
	{
		state = value;
	}
}
