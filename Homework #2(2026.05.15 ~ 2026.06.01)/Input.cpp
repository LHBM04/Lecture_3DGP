#include "Precompiled.h"
#include "Input.h"

namespace
{
	constexpr std::size_t keyCount{ 256 };
	constexpr std::size_t mouseButtonCount{ static_cast<std::size_t>(MouseButton::Count) };

	[[nodiscard]] bool IsValidVirtualKey(int virtualKey_) noexcept
	{
		return 0 <= virtualKey_ && virtualKey_ < static_cast<int>(keyCount);
	}

	[[nodiscard]] std::size_t ToIndex(MouseButton button_) noexcept
	{
		return static_cast<std::size_t>(button_);
	}

	[[nodiscard]] bool IsValidMouseButton(MouseButton button_) noexcept
	{
		return ToIndex(button_) < mouseButtonCount;
	}

}

void Input::BeginFrame() noexcept
{
	previousKeys = currentKeys;
	previousMouseButtons = currentMouseButtons;
	previousMouseX = mouseX;
	previousMouseY = mouseY;
}

bool Input::IsKeyHeld(int virtualKey_) noexcept
{
	return IsValidVirtualKey(virtualKey_) && currentKeys[static_cast<std::size_t>(virtualKey_)];
}

bool Input::IsKeyPressed(int virtualKey_) noexcept
{
	if (!IsValidVirtualKey(virtualKey_))
	{
		return false;
	}

	const std::size_t index{ static_cast<std::size_t>(virtualKey_) };
	return currentKeys[index] && !previousKeys[index];
}

bool Input::IsKeyReleased(int virtualKey_) noexcept
{
	if (!IsValidVirtualKey(virtualKey_))
	{
		return false;
	}

	const std::size_t index{ static_cast<std::size_t>(virtualKey_) };
	return !currentKeys[index] && previousKeys[index];
}

bool Input::IsMouseButtonDown(MouseButton button_) noexcept
{
	return IsValidMouseButton(button_) && currentMouseButtons[ToIndex(button_)];
}

bool Input::IsMouseButtonPressed(MouseButton button_) noexcept
{
	if (!IsValidMouseButton(button_))
	{
		return false;
	}

	const std::size_t index{ ToIndex(button_) };
	return currentMouseButtons[index] && !previousMouseButtons[index];
}

bool Input::IsMouseButtonReleased(MouseButton button_) noexcept
{
	if (!IsValidMouseButton(button_))
	{
		return false;
	}

	const std::size_t index{ ToIndex(button_) };
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

int Input::GetMouseDeltaX() noexcept
{
	return mouseX - previousMouseX;
}

int Input::GetMouseDeltaY() noexcept
{
	return mouseY - previousMouseY;
}

void Input::SetKey(int virtualKey_, bool isDown_) noexcept
{
	if (IsValidVirtualKey(virtualKey_))
	{
		currentKeys[static_cast<std::size_t>(virtualKey_)] = isDown_;
	}
}

void Input::SetMouseButton(MouseButton button_, bool isDown_) noexcept
{
	if (IsValidMouseButton(button_))
	{
		currentMouseButtons[ToIndex(button_)] = isDown_;
	}
}

void Input::SetMousePosition(int x_, int y_) noexcept
{
	mouseX = x_;
	mouseY = y_;
}
