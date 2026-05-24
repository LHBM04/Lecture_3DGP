#include "Precompiled.h"
#include "Input.h"

bool Input::Initialize() noexcept
{
	if (isInitialized)
	{
		return true;
	}

	currentKeys.fill(false);
	previousKeys.fill(false);
	currentMouseButtons.fill(false);
	previousMouseButtons.fill(false);
	mouseX = 0;
	mouseY = 0;
	screenWidth = 1;
	screenHeight = 1;
	isInitialized = true;
	return true;
}

void Input::Release() noexcept
{
	currentKeys.fill(false);
	previousKeys.fill(false);
	currentMouseButtons.fill(false);
	previousMouseButtons.fill(false);
	isInitialized = false;
}

void Input::Update() noexcept
{
	previousKeys = currentKeys;
	previousMouseButtons = currentMouseButtons;
}

void Input::ProcessEvent(const Event& event_) noexcept
{
	switch (event_.type)
	{
	case Event::Type::KeyDown:
		if (0 <= event_.key.keyCode && event_.key.keyCode < static_cast<int>(currentKeys.size()))
		{
			currentKeys[static_cast<std::size_t>(event_.key.keyCode)] = true;
		}
		break;
	case Event::Type::KeyUp:
		if (0 <= event_.key.keyCode && event_.key.keyCode < static_cast<int>(currentKeys.size()))
		{
			currentKeys[static_cast<std::size_t>(event_.key.keyCode)] = false;
		}
		break;
	case Event::Type::MouseMove:
		mouseX = event_.mouseMove.x;
		mouseY = event_.mouseMove.y;
		break;
	case Event::Type::MouseButtonDown:
		if (0 <= event_.mouseButton.button && event_.mouseButton.button < static_cast<int>(currentMouseButtons.size()))
		{
			currentMouseButtons[static_cast<std::size_t>(event_.mouseButton.button)] = true;
			mouseX = event_.mouseButton.x;
			mouseY = event_.mouseButton.y;
		}
		break;
	case Event::Type::MouseButtonUp:
		if (0 <= event_.mouseButton.button && event_.mouseButton.button < static_cast<int>(currentMouseButtons.size()))
		{
			currentMouseButtons[static_cast<std::size_t>(event_.mouseButton.button)] = false;
			mouseX = event_.mouseButton.x;
			mouseY = event_.mouseButton.y;
		}
		break;
	default:
		break;
	}
}

void Input::SetScreenSize(int width_, int height_) noexcept
{
	screenWidth = std::max(1, width_);
	screenHeight = std::max(1, height_);
}

bool Input::IsKeyDown(KeyCode keyCode_) const noexcept
{
	const int keyCode{ static_cast<int>(keyCode_) };
	if (keyCode < 0 || static_cast<int>(currentKeys.size()) <= keyCode)
	{
		return false;
	}

	return currentKeys[static_cast<std::size_t>(keyCode)];
}

bool Input::IsKeyPressed(KeyCode keyCode_) const noexcept
{
	const int keyCode{ static_cast<int>(keyCode_) };
	if (keyCode < 0 || static_cast<int>(currentKeys.size()) <= keyCode)
	{
		return false;
	}

	const std::size_t index{ static_cast<std::size_t>(keyCode) };
	return currentKeys[index] && !previousKeys[index];
}

bool Input::IsKeyReleased(KeyCode keyCode_) const noexcept
{
	const int keyCode{ static_cast<int>(keyCode_) };
	if (keyCode < 0 || static_cast<int>(currentKeys.size()) <= keyCode)
	{
		return false;
	}

	const std::size_t index{ static_cast<std::size_t>(keyCode) };
	return !currentKeys[index] && previousKeys[index];
}

bool Input::IsMouseButtonDown(MouseCode mouseCode_) const noexcept
{
	const int mouseCode{ static_cast<int>(mouseCode_) };
	if (mouseCode < 0 || static_cast<int>(currentMouseButtons.size()) <= mouseCode)
	{
		return false;
	}

	return currentMouseButtons[static_cast<std::size_t>(mouseCode)];
}

bool Input::IsMouseButtonPressed(MouseCode mouseCode_) const noexcept
{
	const int mouseCode{ static_cast<int>(mouseCode_) };
	if (mouseCode < 0 || static_cast<int>(currentMouseButtons.size()) <= mouseCode)
	{
		return false;
	}

	const std::size_t index{ static_cast<std::size_t>(mouseCode) };
	return currentMouseButtons[index] && !previousMouseButtons[index];
}

bool Input::IsMouseButtonReleased(MouseCode mouseCode_) const noexcept
{
	const int mouseCode{ static_cast<int>(mouseCode_) };
	if (mouseCode < 0 || static_cast<int>(currentMouseButtons.size()) <= mouseCode)
	{
		return false;
	}

	const std::size_t index{ static_cast<std::size_t>(mouseCode) };
	return !currentMouseButtons[index] && previousMouseButtons[index];
}

int Input::GetMouseX() const noexcept
{
	return mouseX;
}

int Input::GetMouseY() const noexcept
{
	return mouseY;
}
