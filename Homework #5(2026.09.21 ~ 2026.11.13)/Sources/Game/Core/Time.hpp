#pragma once

#include "Framework/Core.hpp"
#include "Framework/Platform.hpp"

namespace TUK::Game
{
	using namespace Framework;

	namespace Time
	{
		void Initialize(const TimeSubsystem* subsystem) noexcept;
		void Release() noexcept;

		[[nodiscard]] const TimeSubsystem* GetSubsystem() noexcept;

		[[nodiscard]] f32 GetTimeScale() noexcept;

		[[nodiscard]] f32 GetDeltaTime() noexcept;
		[[nodiscard]] f32 GetUnscaledDeltaTime() noexcept;

		[[nodiscard]] f32 GetFixedDeltaTime() noexcept;
		[[nodiscard]] f32 GetUnscaledFixedDeltaTime() noexcept;
	}
}
