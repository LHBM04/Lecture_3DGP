#pragma once

#include "Vector2D.hpp"

class Input final
{
	STATIC_CLASS(Input)
public:
	enum class InputState : unsigned char
	{
		Idle,
		Pressed,
		Held,
		Released
	};

	static void SetKeyState(int key_, InputState state_);
	static void SetMouseButtonState(int button_, InputState state_);

	static void SetMousePosition(const Vector2D& position_);

	[[nodiscard]] static bool IsKeyPressed(int key_);
	[[nodiscard]] static bool IsKeyHeld(int key_);
	[[nodiscard]] static bool IsKeyReleased(int key_);

	[[nodiscard]] static bool IsMouseButtonPressed(int button_);
	[[nodiscard]] static bool IsMouseButtonHeld(int button_);
	[[nodiscard]] static bool IsMouseButtonReleased(int button_);

	[[nodiscard]] static Vector2D GetMousePosition();
	[[nodiscard]] static Vector2D GetMousePositionDelta();

	static void Update();

private:
	static constexpr std::size_t MAX_KEYS = 256;
	static constexpr std::size_t MAX_MOUSE_BUTTONS = 5;

	static std::array<InputState, MAX_KEYS> keyStates;
	static std::array<InputState, MAX_MOUSE_BUTTONS> buttonStates;

	static Vector2D lastMousePosition;
	static Vector2D currentMousePosition;
};