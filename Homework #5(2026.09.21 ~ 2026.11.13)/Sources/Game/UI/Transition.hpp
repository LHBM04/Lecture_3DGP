#pragma once

#include "BaseTransition.hpp"

namespace TUK::Game
{
	class Transition : public BaseTransition
	{
	public:
		using BaseTransition::BaseTransition;

		void OnPlayingUpdate() override;
		void Reset() noexcept override;

		void Open() override;
		void OpenComplete() override;
		void Close() override;
		void CloseComplete() override;

		void Play() override;
		void Pause() override;
		void Stop() override;

		void SetProgress(f32 value) noexcept override;

		[[nodiscard]] f32 GetOpenDuration() const noexcept;
		void SetOpenDuration(f32 duration) noexcept;

		[[nodiscard]] f32 GetCloseDuration() const noexcept;
		void SetCloseDuration(f32 duration) noexcept;

		void SetDuration(f32 duration) noexcept;

		[[nodiscard]] Delegate<>& OnOpenBegin() noexcept;
		[[nodiscard]] Delegate<>& OnOpenComplete() noexcept;
		[[nodiscard]] Delegate<>& OnCloseBegin() noexcept;
		[[nodiscard]] Delegate<>& OnCloseComplete() noexcept;
		[[nodiscard]] Delegate<>& OnPlay() noexcept;
		[[nodiscard]] Delegate<>& OnPause() noexcept;
		[[nodiscard]] Delegate<>& OnStop() noexcept;
		[[nodiscard]] Delegate<f32>& OnProgressChanged() noexcept;

	private:
		static constexpr f32 MAX_DELTA_TIME = 1.0f / 30.0f;

		f32 openDuration = 0.35f;
		f32 closeDuration = 0.35f;

		Delegate<> onOpenBegin;
		Delegate<> onOpenComplete;
		Delegate<> onCloseBegin;
		Delegate<> onCloseComplete;
		Delegate<> onPlay;
		Delegate<> onPause;
		Delegate<> onStop;
		Delegate<f32> onProgressChanged;
	};

	inline f32 Transition::GetOpenDuration() const noexcept
	{
		return openDuration;
	}

	inline void Transition::SetOpenDuration(f32 duration) noexcept
	{
		if (std::isfinite(duration) && duration >= 0.0f)
		{
			openDuration = duration;
		}
	}

	inline f32 Transition::GetCloseDuration() const noexcept
	{
		return closeDuration;
	}

	inline void Transition::SetCloseDuration(f32 duration) noexcept
	{
		if (std::isfinite(duration) && duration >= 0.0f)
		{
			closeDuration = duration;
		}
	}

	inline void Transition::SetDuration(f32 duration) noexcept
	{
		SetOpenDuration(duration);
		SetCloseDuration(duration);
	}

	inline Delegate<>& Transition::OnOpenBegin() noexcept
	{
		return onOpenBegin;
	}

	inline Delegate<>& Transition::OnOpenComplete() noexcept
	{
		return onOpenComplete;
	}

	inline Delegate<>& Transition::OnCloseBegin() noexcept
	{
		return onCloseBegin;
	}

	inline Delegate<>& Transition::OnCloseComplete() noexcept
	{
		return onCloseComplete;
	}

	inline Delegate<>& Transition::OnPlay() noexcept
	{
		return onPlay;
	}

	inline Delegate<>& Transition::OnPause() noexcept
	{
		return onPause;
	}

	inline Delegate<>& Transition::OnStop() noexcept
	{
		return onStop;
	}

	inline Delegate<f32>& Transition::OnProgressChanged() noexcept
	{
		return onProgressChanged;
	}
}
