#include "Precompiled.hpp"
#include "Input.hpp"

void Input::Update()
{
	for (int index = 0; index < MAX_KEYS; ++index)
	{
		if (keyStates[index] == InputState::Pressed)
		{
			keyStates[index] = InputState::Held;
		}

		if (keyStates[index] == InputState::Released)
		{
			keyStates[index] = InputState::Idle;
		}
	}

	for (int index = 0; index < MAX_MOUSE_BUTTONS; ++index)
	{
		if (buttonStates[index] == InputState::Pressed)
		{
			buttonStates[index] = InputState::Held;
		}

		if (buttonStates[index] == InputState::Released)
		{
			buttonStates[index] = InputState::Idle;
		}
	}
}

void Input::SetKeyState(int key_, InputState state_)
{
	keyStates.at(key_) = state_;
}

void Input::SetMouseButtonState(int button_, InputState state_)
{
	buttonStates.at(button_) = state_;
}

void Input::SetMousePosition(const Vector2D& position_)
{
	currentMousePosition = position_;
}

bool Input::IsKeyPressed(int key_)
{
	return keyStates.at(key_) == InputState::Pressed;
}

bool Input::IsKeyHeld(int key_)
{
	return keyStates.at(key_) == InputState::Held;
}

bool Input::IsKeyReleased(int key_)
{
	return keyStates.at(key_) == InputState::Released;
}

bool Input::IsMouseButtonPressed(int button_)
{
	return buttonStates.at(button_) == InputState::Pressed;
}

bool Input::IsMouseButtonHeld(int button_)
{
	return buttonStates.at(button_) == InputState::Held;
}

bool Input::IsMouseButtonReleased(int button_)
{
	return buttonStates.at(button_) == InputState::Released;
}

Vector2D Input::GetMousePosition()
{
	return currentMousePosition;
}

Vector2D Input::GetMousePositionDelta()
{
	return currentMousePosition - lastMousePosition;
}

std::array<Input::InputState, Input::MAX_KEYS> Input::keyStates{};
std::array<Input::InputState, Input::MAX_MOUSE_BUTTONS> Input::buttonStates{};

Vector2D Input::lastMousePosition{};
Vector2D Input::currentMousePosition{};