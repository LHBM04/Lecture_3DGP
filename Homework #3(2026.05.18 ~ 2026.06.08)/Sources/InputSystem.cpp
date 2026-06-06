#include "Precompiled.h"
#include "InputSystem.h"

void InputSystem::Reset()
{
	keyStates.fill(false);
	lastKeyStates.fill(false);

	buttonStates.fill(false);
	lastButtonStates.fill(false);

	mousePosition = { 0.0f, 0.0f };
	lastMousePosition = { 0.0f, 0.0f };
}

void InputSystem::Update()
{
	lastKeyStates = keyStates;
	lastButtonStates = buttonStates;

	BYTE keyboardState[256];
	if (GetKeyboardState(keyboardState))
	{
		for (std::size_t index{ 0 }; index < 256; ++index)
		{
			keyStates[index] = (keyboardState[index] & 0x80) != 0;
		}
	}

	buttonStates[std::to_underlying(ButtonCode::Left)] = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
	buttonStates[std::to_underlying(ButtonCode::Right)] = (GetKeyState(VK_RBUTTON) & 0x8000) != 0;
	buttonStates[std::to_underlying(ButtonCode::Middle)] = (GetKeyState(VK_MBUTTON) & 0x8000) != 0;

	HWND hWnd{ ::GetForegroundWindow() };
	if (hWnd != nullptr)
	{
		POINT pt;
		GetCursorPos(&pt);

		POINT clientPt = pt;
		ScreenToClient(hWnd, &clientPt);

		lastMousePosition = mousePosition;
		mousePosition.x = static_cast<float>(clientPt.x);
		mousePosition.y = static_cast<float>(clientPt.y);
	}
}

bool InputSystem::IsKeyPressed(KeyCode key_) const noexcept
{
	const std::uint8_t index{ std::to_underlying(key_) };
	return keyStates[index] && !lastKeyStates[index];
}

bool InputSystem::IsKeyDown(KeyCode key_) const noexcept
{
	const std::uint8_t index{ std::to_underlying(key_) };
	return keyStates[index];
}

bool InputSystem::IsKeyUp(KeyCode key_) const noexcept
{
	const std::uint8_t index{ std::to_underlying(key_) };
	return !keyStates[index] && lastButtonStates[index];
}

bool InputSystem::IsButtonPressed(ButtonCode button_) const noexcept
{
	const std::uint8_t index{ std::to_underlying(button_) };
	return buttonStates[index] && !lastButtonStates[index];
}

bool InputSystem::IsButtonDown(ButtonCode button_) const noexcept
{
	const std::uint8_t index{ std::to_underlying(button_) };
	return buttonStates[index];
}

bool InputSystem::IsButtonUp(ButtonCode button_) const noexcept
{
	const std::uint8_t index{ std::to_underlying(button_) };
	return !buttonStates[index] && lastButtonStates[index];
}

float InputSystem::GetMousePositionX() const noexcept
{
	return mousePosition.x;
}

float InputSystem::GetMousePositionY() const noexcept
{
	return mousePosition.y;
}

const Vector2D& InputSystem::GetMousePosition() const noexcept
{
	return mousePosition;
}

const Vector2D& InputSystem::GetMousePositionDelta() const noexcept
{
	return mousePosition - lastMousePosition;
}
