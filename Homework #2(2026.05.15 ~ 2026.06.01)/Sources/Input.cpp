#include "Precompiled.h"
#include "Input.h"

namespace
{
	constexpr std::size_t KEY_COUNT{ 256 };
	constexpr std::size_t BUTTON_COUNT{ };

	std::array<bool, KEY_COUNT> currentKeys{};
	std::array<bool, KEY_COUNT> previousKeys{};

	std::array<bool, BUTTON_COUNT> currentMouseButtons{};
	std::array<bool, BUTTON_COUNT> previousMouseButtons{};

	int mouseX{ 0 };
	int mouseY{ 0 };
	int previousMouseX{ 0 };
	int previousMouseY{ 0 };
}

bool Input::IsKeyDown(KeyCode keyCode_) noexcept
{
	return currentKeys[(std::size_t)keyCode_];
}

bool Input::IsKeyPressed(KeyCode keyCode_) noexcept
{
	const std::size_t index{ (std::size_t)keyCode_ };
	return currentKeys[index] && !previousKeys[index];
}

bool Input::IsKeyReleased(KeyCode keyCode_) noexcept
{
	const std::size_t index{ (std::size_t)keyCode_ };
	return !currentKeys[index] && previousKeys[index];
}

bool Input::IsMouseButtonDown(ButtonCode button_) noexcept
{
	return currentMouseButtons[(std::size_t)button_];
}

bool Input::IsMouseButtonPressed(ButtonCode button_) noexcept
{
	const std::size_t index{ (std::size_t)button_ };
	return currentMouseButtons[index] && !previousMouseButtons[index];
}

bool Input::IsMouseButtonReleased(ButtonCode button_) noexcept
{
	const std::size_t index{ (std::size_t)button_ };
	return !currentMouseButtons[index] && previousMouseButtons[index];
}

int Input::GetMouseX() noexcept
{
	return mouseX;
}

int Input::GetMouseY() noexcept
{
	return mouseY;
}

std::pair<int, int> Input::GetMousePosition() noexcept
{
	return std::pair<int, int>(mouseX, mouseY);
}

int Input::GetMouseDeltaX() noexcept
{
	return mouseX - previousMouseX;
}

int Input::GetMouseDeltaY() noexcept
{
	return mouseY - previousMouseY;
}

std::pair<int, int> Input::GetMouseDelta() noexcept
{
	return std::pair<int, int>(mouseX - previousMouseX, mouseY - previousMouseY);
}

void Input::Update() noexcept
{
	previousKeys = currentKeys;
	previousMouseButtons = currentMouseButtons;
	previousMouseX = mouseX;
	previousMouseY = mouseY;
}
