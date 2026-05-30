#include "Precompiled.h"
#include "InputSystem.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void InputSystem::Reset()
{
	keyStates.fill(false);
	prevKeyStates.fill(false);
	
	buttonStates.fill(false);
	prevButtonStates.fill(false);

	mousePosition = { 0, 0 };
}

void InputSystem::Update()
{
	prevKeyStates = keyStates;
	prevButtonStates = buttonStates;

	// Keyboard State
	BYTE keyboardState[256];
	if (GetKeyboardState(keyboardState))
	{
		for (int i = 0; i < 256; ++i)
		{
			keyStates[i] = (keyboardState[i] & 0x80) != 0;
		}
	}

	// Mouse State
	buttonStates[std::to_underlying(ButtonCode::Left)] = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
	buttonStates[std::to_underlying(ButtonCode::Right)] = (GetKeyState(VK_RBUTTON) & 0x8000) != 0;
	buttonStates[std::to_underlying(ButtonCode::Middle)] = (GetKeyState(VK_MBUTTON) & 0x8000) != 0;

	// Mouse Position
	POINT pt;
	GetCursorPos(&pt);
	
	// Convert to screen coordinates
	HWND hWnd{ GetActiveWindow() };
	if (hWnd != nullptr)
	{
		ScreenToClient(hWnd, &pt);
		mousePosition.x = static_cast<float>(pt.x);
		mousePosition.y = static_cast<float>(pt.y);
	}
}

