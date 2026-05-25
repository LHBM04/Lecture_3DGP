#include "Precompiled.h"
#include "InputManager.h"

#include "Event.h"

namespace
{
	constexpr std::size_t KeyCount{ 256 };
	constexpr std::size_t MouseButtonCount{ 5 };

	std::array<bool, KeyCount> currentKeys;
	std::array<bool, KeyCount> previousKeys;

	std::array<bool, MouseButtonCount> currentMouseButtons;
	std::array<bool, MouseButtonCount> previousMouseButtons;

	int screenWidth;
	int screenHeight;

	int mouseX;
	int mouseY;
}

void InputManager::Reset() noexcept
{
	currentKeys.fill(false);
	previousKeys.fill(false);

	currentMouseButtons.fill(false);
	previousMouseButtons.fill(false);
	
	mouseX = 0;
	mouseY = 0;
	
	screenWidth = 1;
	screenHeight = 1;
}

void InputManager::Update() noexcept
{
	previousKeys = currentKeys;
	previousMouseButtons = currentMouseButtons;
}

void InputManager::ProcessEvent(const Event& event_) noexcept
{
	switch (event_.type)
	{
	case Event::Type::KeyDown:
	{
		currentKeys[static_cast<std::size_t>(event_.key.keyCode)] = true;
		break;
	}
	case Event::Type::KeyUp:
	{
		currentKeys[static_cast<std::size_t>(event_.key.keyCode)] = false;
		break;
	}
	case Event::Type::MouseMove:
	{
		mouseX = event_.mouseMove.x;
		mouseY = event_.mouseMove.y;
		break;
	}
	case Event::Type::MouseButtonDown:
	{
		currentMouseButtons[static_cast<std::size_t>(event_.mouseButton.button)] = true;
		mouseX = event_.mouseButton.x;
		mouseY = event_.mouseButton.y;
		break;
	}
	case Event::Type::MouseButtonUp:
	{
		currentMouseButtons[static_cast<std::size_t>(event_.mouseButton.button)] = false;
		mouseX = event_.mouseButton.x;
		mouseY = event_.mouseButton.y;
		break;
	}
	default:
	{
		break;
	}
	}
}

bool InputManager::IsKeyDown(KeyCode keyCode_) noexcept
{
	const int keyCode{ (int)keyCode_ };
	assert(keyCode >= 0 && keyCode < static_cast<int>(KeyCount));

	return currentKeys[keyCode];
}

bool InputManager::IsKeyPressed(KeyCode keyCode_) noexcept
{
	const int keyCode{ (int)keyCode_ };
	assert(keyCode >= 0 && keyCode < static_cast<int>(KeyCount));

	return currentKeys[keyCode] && !previousKeys[keyCode];
}

bool InputManager::IsKeyReleased(KeyCode keyCode_) noexcept
{
	const int keyCode{ (int)keyCode_ };
	assert(keyCode >= 0 && keyCode < static_cast<int>(KeyCount));

	return !currentKeys[keyCode] && previousKeys[keyCode];
}

bool InputManager::IsButtonDown(ButtonCode mouseCode_) noexcept
{
	const int mouseCode{ (int)mouseCode_ };
	assert(mouseCode >= 0 && mouseCode < static_cast<int>(MouseButtonCount));

	return currentMouseButtons[static_cast<std::size_t>(mouseCode)];
}

bool InputManager::IsButtonPressed(ButtonCode mouseCode_) noexcept
{
	const int mouseCode{ (int)mouseCode_ };
	assert(mouseCode >= 0 && mouseCode < static_cast<int>(MouseButtonCount));

	return currentMouseButtons[mouseCode] && !previousMouseButtons[mouseCode];
}

bool InputManager::IsButtonReleased(ButtonCode mouseCode_) noexcept
{
	const int mouseCode{ (int)mouseCode_ };
	assert(mouseCode >= 0 && mouseCode < static_cast<int>(MouseButtonCount));

	return !currentMouseButtons[mouseCode] && previousMouseButtons[mouseCode];
}

int InputManager::GetMousePositionX() noexcept
{
	return mouseX;
}

void InputManager::SetMousePositionX(int x_) noexcept
{
	mouseX = x_;
}

int InputManager::GetMousePositionY() noexcept
{
	return mouseY;
}

void InputManager::SetMousePositionY(int y_) noexcept
{
	mouseY = y_;
}

std::pair<int, int> InputManager::GetMousePosition() noexcept
{
	return { mouseX, mouseY };
}

void InputManager::SetMousePosition(int x_, int y_) noexcept
{
	mouseX = x_;
	mouseY = y_;
}

std::pair<int, int> InputManager::GetScreenSize() noexcept
{
	return { screenWidth, screenHeight };
}

void InputManager::SetScreenSize(int width_, int height_) noexcept
{
	screenWidth = std::max(1, width_);
	screenHeight = std::max(1, height_);
}
