#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <GameInput.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	enum class KeyCode : u8
	{
		NONE = 0x00,
		BACKSPACE = VK_BACK,
		TAB = VK_TAB,
		ENTER = VK_RETURN,
		SHIFT = VK_SHIFT,
		CONTROL = VK_CONTROL,
		ALT = VK_MENU,
		PAUSE = VK_PAUSE,
		CAPS_LOCK = VK_CAPITAL,
		ESCAPE = VK_ESCAPE,
		SPACE = VK_SPACE,
		PAGE_UP = VK_PRIOR,
		PAGE_DOWN = VK_NEXT,
		END = VK_END,
		HOME = VK_HOME,
		LEFT = VK_LEFT,
		UP = VK_UP,
		RIGHT = VK_RIGHT,
		DOWN = VK_DOWN,
		PRINT_SCREEN = VK_SNAPSHOT,
		INSERT = VK_INSERT,
		DELETE_KEY = VK_DELETE,

		D0 = '0',
		D1 = '1',
		D2 = '2',
		D3 = '3',
		D4 = '4',
		D5 = '5',
		D6 = '6',
		D7 = '7',
		D8 = '8',
		D9 = '9',

		A = 'A',
		B = 'B',
		C = 'C',
		D = 'D',
		E = 'E',
		F = 'F',
		G = 'G',
		H = 'H',
		I = 'I',
		J = 'J',
		K = 'K',
		L = 'L',
		M = 'M',
		N = 'N',
		O = 'O',
		P = 'P',
		Q = 'Q',
		R = 'R',
		S = 'S',
		T = 'T',
		U = 'U',
		V = 'V',
		W = 'W',
		X = 'X',
		Y = 'Y',
		Z = 'Z',

		LEFT_SUPER = VK_LWIN,
		RIGHT_SUPER = VK_RWIN,

		NUMPAD0 = VK_NUMPAD0,
		NUMPAD1 = VK_NUMPAD1,
		NUMPAD2 = VK_NUMPAD2,
		NUMPAD3 = VK_NUMPAD3,
		NUMPAD4 = VK_NUMPAD4,
		NUMPAD5 = VK_NUMPAD5,
		NUMPAD6 = VK_NUMPAD6,
		NUMPAD7 = VK_NUMPAD7,
		NUMPAD8 = VK_NUMPAD8,
		NUMPAD9 = VK_NUMPAD9,
		MULTIPLY = VK_MULTIPLY,
		ADD = VK_ADD,
		SEPARATOR = VK_SEPARATOR,
		SUBTRACT = VK_SUBTRACT,
		DECIMAL = VK_DECIMAL,
		DIVIDE = VK_DIVIDE,

		F1 = VK_F1,
		F2 = VK_F2,
		F3 = VK_F3,
		F4 = VK_F4,
		F5 = VK_F5,
		F6 = VK_F6,
		F7 = VK_F7,
		F8 = VK_F8,
		F9 = VK_F9,
		F10 = VK_F10,
		F11 = VK_F11,
		F12 = VK_F12,

		NUM_LOCK = VK_NUMLOCK,
		SCROLL_LOCK = VK_SCROLL,

		LEFT_SHIFT = VK_LSHIFT,
		RIGHT_SHIFT = VK_RSHIFT,
		LEFT_CONTROL = VK_LCONTROL,
		RIGHT_CONTROL = VK_RCONTROL,
		LEFT_ALT = VK_LMENU,
		RIGHT_ALT = VK_RMENU
	};

	enum class ButtonCode : u32
	{
		NONE = GameInputMouseNone,
		LEFT = GameInputMouseLeftButton,
		RIGHT = GameInputMouseRightButton,
		MIDDLE = GameInputMouseMiddleButton,
		X1 = GameInputMouseButton4,
		X2 = GameInputMouseButton5,
		WHEEL_TILT_LEFT = GameInputMouseWheelTiltLeft,
		WHEEL_TILT_RIGHT = GameInputMouseWheelTiltRight
	};
}
