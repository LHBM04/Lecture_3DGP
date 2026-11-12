#pragma once

#include "Framework/Platform.hpp"

namespace TUK::Game
{
	using namespace Framework;

	/** 편하게 쓰기 위한 래퍼 */
	namespace Input
	{
		void Initialize(InputSubsystem* newSubsystem) noexcept;
		void Release() noexcept;

		[[nodiscard]] InputAction* AddAction(std::string_view name);
		void RemoveAction(std::string_view name) noexcept;

		[[nodiscard]] InputAction* GetAction(std::string_view name) noexcept;

		[[nodiscard]] bool IsKeyPressed(KeyCode keyCode) noexcept;
		[[nodiscard]] bool IsKeyHeld(KeyCode keyCode) noexcept;
		[[nodiscard]] bool IsKeyReleased(KeyCode keyCode) noexcept;

		[[nodiscard]] bool IsMouseButtonPressed(ButtonCode button) noexcept;
		[[nodiscard]] bool IsMouseButtonHeld(ButtonCode button) noexcept;
		[[nodiscard]] bool IsMouseButtonReleased(ButtonCode button) noexcept;

		[[nodiscard]] f32 GetHorizontal(KeyCode left, KeyCode right) noexcept;
		[[nodiscard]] f32 GetVertical(KeyCode up, KeyCode down) noexcept;

		[[nodiscard]] Vector2D<i32> GetMousePosition() noexcept;
		[[nodiscard]] Vector2D<i32> GetMousePositionDelta() noexcept;
	}
}
