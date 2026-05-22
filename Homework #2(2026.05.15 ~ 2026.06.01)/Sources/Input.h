#pragma once

enum class KeyCode : unsigned char
{

};

enum class ButtonCode : unsigned char
{
	Left,
	Right,
	Middle,
	X1,
	X2,
	Count
};

namespace Input
{
	[[nodiscard]] bool IsKeyDown(KeyCode virtualKey_) noexcept;
	[[nodiscard]] bool IsKeyPressed(KeyCode virtualKey_) noexcept;
	[[nodiscard]] bool IsKeyReleased(KeyCode virtualKey_) noexcept;

	[[nodiscard]] bool IsMouseButtonDown(ButtonCode button_) noexcept;
	[[nodiscard]] bool IsMouseButtonPressed(ButtonCode button_) noexcept;
	[[nodiscard]] bool IsMouseButtonReleased(ButtonCode button_) noexcept;

	[[nodiscard]] int GetMouseX() noexcept;
	[[nodiscard]] int GetMouseY() noexcept;
	[[nodiscard]] std::pair<int, int> GetMousePosition() noexcept;

	[[nodiscard]] int GetMouseDeltaX() noexcept;
	[[nodiscard]] int GetMouseDeltaY() noexcept;
	[[nodiscard]] std::pair<int, int> GetMouseDelta() noexcept;

	void Update();
}
