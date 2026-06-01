#include "Precompiled.h"
#include "InputSystem.h"

void InputSystem::Reset()
{
	keyStates.fill(false);
	prevKeyStates.fill(false);
	
	buttonStates.fill(false);
	prevButtonStates.fill(false);

	mousePosition = { 0.0f, 0.0f };
	prevMousePosition = { 0.0f, 0.0f };

	isCursorLocked = false;
}

void InputSystem::Update()
{
	prevKeyStates = keyStates;
	prevButtonStates = buttonStates;

	// Keyboard State
	BYTE keyboardState[256];
	if (GetKeyboardState(keyboardState))
	{
		for (int i{ 0 }; i < 256; ++i)
		{
			keyStates[i] = (keyboardState[i] & 0x80) != 0;
		}
	}

	// Mouse State
	buttonStates[std::to_underlying(ButtonCode::Left)] = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
	buttonStates[std::to_underlying(ButtonCode::Right)] = (GetKeyState(VK_RBUTTON) & 0x8000) != 0;
	buttonStates[std::to_underlying(ButtonCode::Middle)] = (GetKeyState(VK_MBUTTON) & 0x8000) != 0;

	// Mouse Position
	HWND hWnd{ GetActiveWindow() };
	if (hWnd != nullptr)
	{
		POINT pt;
		GetCursorPos(&pt);
		
		POINT clientPt = pt;
		ScreenToClient(hWnd, &clientPt);
		
		prevMousePosition = mousePosition;
		mousePosition.x = static_cast<float>(clientPt.x);
		mousePosition.y = static_cast<float>(clientPt.y);

		if (isCursorLocked)
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			int centerX{ (rect.right - rect.left) / 2 };
			int centerY{ (rect.bottom - rect.top) / 2 };

			POINT centerPt{ centerX, centerY };
			ClientToScreen(hWnd, &centerPt);
			SetCursorPos(centerPt.x, centerPt.y);
			
			// Next frame's prev position will be the center
			mousePosition.x = static_cast<float>(centerX);
			mousePosition.y = static_cast<float>(centerY);
		}
	}
}

bool InputSystem::IsKeyPressed(KeyCode key_) const
{
	const std::uint8_t index{ std::to_underlying(key_) };
	return keyStates[index] && !prevKeyStates[index];
}

bool InputSystem::IsKeyDown(KeyCode key_) const
{
	const std::uint8_t index{ std::to_underlying(key_) };
	return keyStates[index];
}

bool InputSystem::IsKeyUp(KeyCode key_) const
{
	const std::uint8_t index{ std::to_underlying(key_) };
	return !keyStates[index] && prevKeyStates[index];
}

bool InputSystem::IsButtonPressed(ButtonCode button_) const
{
	const std::uint8_t index{ std::to_underlying(button_) };
	return buttonStates[index] && !prevButtonStates[index];
}

bool InputSystem::IsButtonDown(ButtonCode button_) const
{
	const std::uint8_t index{ std::to_underlying(button_) };
	return buttonStates[index];
}

bool InputSystem::IsButtonUp(ButtonCode button_) const
{
	const std::uint8_t index{ std::to_underlying(button_) };
	return !buttonStates[index] && prevButtonStates[index];
}

const Vector2D& InputSystem::GetMousePosition() const
{
	return mousePosition;
}

Vector2D InputSystem::GetMouseDelta() const
{
	return Vector2D{ mousePosition.x - prevMousePosition.x, mousePosition.y - prevMousePosition.y };
}

void InputSystem::SetCursorLock(bool isLocked_)
{
	if (isCursorLocked == isLocked_)
	{
		return;
	}

	isCursorLocked = isLocked_;

	if (isCursorLocked)
	{
		while (ShowCursor(FALSE) >= 0);
	}
	else
	{
		while (ShowCursor(TRUE) < 0);
	}
}

bool InputSystem::IsCursorLocked() const noexcept
{
	return isCursorLocked;
}
