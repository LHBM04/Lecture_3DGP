#pragma once

#include "Framework/Platform.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	/** 편하게 쓰기 위한 래퍼 */
	namespace Input
	{
		void Initialize(const InputProvider* newProvider);
		void Release();

		[[nodiscard]] bool IsKeyPressed(KeyCode keyCode);
		[[nodiscard]] bool IsKeyHeld(KeyCode keyCode);
		[[nodiscard]] bool IsKeyReleased(KeyCode keyCode);

		[[nodiscard]] bool IsMouseButtonPressed(ButtonCode button);
		[[nodiscard]] bool IsMouseButtonHeld(ButtonCode button);
		[[nodiscard]] bool IsMouseButtonReleased(ButtonCode button);

		[[nodiscard]] f32 GetHorizontal(KeyCode left, KeyCode right);
		[[nodiscard]] f32 GetVertical(KeyCode up, KeyCode down);

		[[nodiscard]] Vector2D<i32> GetMousePosition() noexcept;
		[[nodiscard]] Vector2D<i32> GetMousePositionDelta() noexcept;
	}
}
