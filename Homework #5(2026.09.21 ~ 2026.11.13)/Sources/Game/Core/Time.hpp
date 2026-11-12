#pragma once

#include "Framework/Core.hpp"
#include "Framework/Platform.hpp"

namespace TUK::Game
{
	using namespace Framework;

	namespace Time
	{
		void Initialize(const TimeSubsystem* subsystem);
		void Release();

		[[nodiscard]] const TimeSubsystem* GetSubsystem() noexcept;

		[[nodiscard]] f32 GetTimeScale();

		[[nodiscard]] f32 GetDeltaTime();
		[[nodiscard]] f32 GetUnscaledDeltaTime();

		[[nodiscard]] f32 GetFixedDeltaTime();
		[[nodiscard]] f32 GetUnscaledFixedDeltaTime();
	}
}
