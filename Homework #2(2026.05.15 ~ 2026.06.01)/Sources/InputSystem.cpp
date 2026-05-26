#include "Precompiled.h"
#include "InputSystem.h"

#include "Event.h"
#include <algorithm>
#include <cassert>

bool InputSystem::Initialize(const InputOptions& options_)
{
	screenWidth = std::max(1, options_.screenWidth);
	screenHeight = std::max(1, options_.screenHeight);
	dispatchKeys = options_.dispatchKeys;
	Reset();
	return true;
}

void InputSystem::Release()
{
	dispatchKeys.clear();
}

void InputSystem::Reset() noexcept
{
	currentKeys.fill(false);
	previousKeys.fill(false);

	currentMouseButtons.fill(false);
	previousMouseButtons.fill(false);
	
	mouseX = 0;
	mouseY = 0;
}

void InputSystem::Update() noexcept
{
	previousKeys = currentKeys;
	previousMouseButtons = currentMouseButtons;
}

void InputSystem::ProcessEvent(const Event& event_) noexcept
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

std::span<const KeyCode> InputSystem::GetDispatchKeys() const noexcept
{
	return dispatchKeys;
}

bool InputSystem::IsKeyDown(KeyCode keyCode_) const noexcept
{
	const int keyCode{ static_cast<int>(keyCode_) };
	assert(keyCode >= 0 && keyCode < static_cast<int>(KeyCount));

	return currentKeys[keyCode];
}

bool InputSystem::IsKeyPressed(KeyCode keyCode_) const noexcept
{
	const int keyCode{ static_cast<int>(keyCode_) };
	assert(keyCode >= 0 && keyCode < static_cast<int>(KeyCount));

	return currentKeys[keyCode] && !previousKeys[keyCode];
}

bool InputSystem::IsKeyReleased(KeyCode keyCode_) const noexcept
{
	const int keyCode{ static_cast<int>(keyCode_) };
	assert(keyCode >= 0 && keyCode < static_cast<int>(KeyCount));

	return !currentKeys[keyCode] && previousKeys[keyCode];
}

bool InputSystem::IsButtonDown(ButtonCode mouseCode_) const noexcept
{
	const int mouseCode{ static_cast<int>(mouseCode_) };
	assert(mouseCode >= 0 && mouseCode < static_cast<int>(MouseButtonCount));

	return currentMouseButtons[static_cast<std::size_t>(mouseCode)];
}

bool InputSystem::IsButtonPressed(ButtonCode mouseCode_) const noexcept
{
	const int mouseCode{ static_cast<int>(mouseCode_) };
	assert(mouseCode >= 0 && mouseCode < static_cast<int>(MouseButtonCount));

	return currentMouseButtons[mouseCode] && !previousMouseButtons[mouseCode];
}

bool InputSystem::IsButtonReleased(ButtonCode mouseCode_) const noexcept
{
	const int mouseCode{ static_cast<int>(mouseCode_) };
	assert(mouseCode >= 0 && mouseCode < static_cast<int>(MouseButtonCount));

	return !currentMouseButtons[mouseCode] && previousMouseButtons[mouseCode];
}

int InputSystem::GetMousePositionX() const noexcept
{
	return mouseX;
}

void InputSystem::SetMousePositionX(int x_) noexcept
{
	mouseX = x_;
}

int InputSystem::GetMousePositionY() const noexcept
{
	return mouseY;
}

void InputSystem::SetMousePositionY(int y_) noexcept
{
	mouseY = y_;
}

std::pair<int, int> InputSystem::GetMousePosition() const noexcept
{
	return { mouseX, mouseY };
}

void InputSystem::SetMousePosition(int x_, int y_) noexcept
{
	mouseX = x_;
	mouseY = y_;
}

std::pair<int, int> InputSystem::GetScreenSize() const noexcept
{
	return { screenWidth, screenHeight };
}

void InputSystem::SetScreenSize(int width_, int height_) noexcept
{
	screenWidth = std::max(1, width_);
	screenHeight = std::max(1, height_);
}
