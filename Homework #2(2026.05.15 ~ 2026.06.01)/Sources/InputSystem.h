#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "Singleton.h"

enum class KeyCode : std::uint8_t
{
	None = 0,

	Backspace = 0x08,
	Tab = 0x09,
	Enter = 0x0D,
	Shift = 0x10,
	Control = 0x11,
	Alt = 0x12,
	Escape = 0x1B,
	Space = 0x20,

	Left = 0x25,
	Up = 0x26,
	Right = 0x27,
	Down = 0x28,

	Alpha0 = '0',
	Alpha1 = '1',
	Alpha2 = '2',
	Alpha3 = '3',
	Alpha4 = '4',
	Alpha5 = '5',
	Alpha6 = '6',
	Alpha7 = '7',
	Alpha8 = '8',
	Alpha9 = '9',

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
};

enum class ButtonCode : std::uint8_t
{
	Left = 0,
	Right = 1,
	Middle = 2,
	XButton1 = 3,
	XButton2 = 4,
};

class InputSystem final : public Singleton<InputSystem>
{
public:
	InputSystem() = default;
	~InputSystem() override = default;

	void Reset();
	void Update();

	[[nodiscard]] bool IsKeyPressed(KeyCode key_) const;
	[[nodiscard]] bool IsKeyDown(KeyCode key_) const;
	[[nodiscard]] bool IsKeyUp(KeyCode key_) const;

	[[nodiscard]] bool IsButtonPressed(ButtonCode button_) const;
	[[nodiscard]] bool IsButtonDown(ButtonCode button_) const;
	[[nodiscard]] bool IsButtonUp(ButtonCode button_) const;

private:
	static constexpr std::size_t KeyCount{ 256 };
	static constexpr std::size_t ButtonCount{ 5 };

	std::array<bool, KeyCount> keyStates;
	std::array<bool, KeyCount> prevKeyStates;

	std::array<bool, ButtonCount> buttonStates;
	std::array<bool, ButtonCount> prevButtonStates;
};

inline bool InputSystem::IsKeyPressed(KeyCode key_) const
{
	const auto index{ std::to_underlying(key_) };
	return keyStates[index] && !prevKeyStates[index];
}

inline bool InputSystem::IsKeyDown(KeyCode key_) const
{
	const auto index{ std::to_underlying(key_) };
	return keyStates[index];
}

inline bool InputSystem::IsKeyUp(KeyCode key_) const
{
	const auto index{ std::to_underlying(key_) };
	return !keyStates[index] && prevKeyStates[index];
}

inline bool InputSystem::IsButtonPressed(ButtonCode button_) const
{
	const auto index{ std::to_underlying(button_) };
	return buttonStates[index] && !prevButtonStates[index];
}

inline bool InputSystem::IsButtonDown(ButtonCode button_) const
{
	const auto index{ std::to_underlying(button_) };
	return buttonStates[index];
}

inline bool InputSystem::IsButtonUp(ButtonCode button_) const
{
	const auto index{ std::to_underlying(button_) };
	return !buttonStates[index] && prevButtonStates[index];
}
