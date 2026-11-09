#pragma once

#include "Framework/Core.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	namespace Time
	{
		void Initialize();
		void Release();

		[[nodiscard]] f32 GetTimeScale();

		[[nodiscard]] f32 GetDeltaTime();
		[[nodiscard]] f32 GetUnscaledDeltaTime();

		[[nodiscard]] f32 GetFixedDeltaTime();
		[[nodiscard]] f32 GetUnscaledFixedDeltaTime();
	}
}
