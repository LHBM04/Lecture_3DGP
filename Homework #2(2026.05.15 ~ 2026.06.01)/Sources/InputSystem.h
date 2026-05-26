#pragma once

#include "System.h"
#include <array>
#include <utility>
#include <span>
#include <vector>

struct Event;

enum class KeyCode : unsigned char
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

enum class ButtonCode : unsigned char
{
	Left = 0,
	Right = 1,
	Middle = 2,
	XButton1 = 3,
	XButton2 = 4,
};

struct InputOptions final
{
	int screenWidth{ 1 };
	int screenHeight{ 1 };
	std::vector<KeyCode> dispatchKeys;
};

class InputSystem final : public System
{
public:
	static constexpr std::size_t KeyCount{ 256 };
	static constexpr std::size_t MouseButtonCount{ 5 };

	bool Initialize(const InputOptions& options_);
	void Release() override;

	void Reset() noexcept;
	void Update() noexcept;
	void ProcessEvent(const Event& event_) noexcept;

	[[nodiscard]] std::span<const KeyCode> GetDispatchKeys() const noexcept;

	[[nodiscard]] bool IsKeyDown(KeyCode keyCode_) const noexcept;
	[[nodiscard]] bool IsKeyPressed(KeyCode keyCode_) const noexcept;
	[[nodiscard]] bool IsKeyReleased(KeyCode keyCode_) const noexcept;

	[[nodiscard]] bool IsButtonDown(ButtonCode buttonCode_) const noexcept;
	[[nodiscard]] bool IsButtonPressed(ButtonCode buttonCode_) const noexcept;
	[[nodiscard]] bool IsButtonReleased(ButtonCode buttonCode_) const noexcept;

	[[nodiscard]] int GetMousePositionX() const noexcept;
	void SetMousePositionX(int x_) noexcept;

	[[nodiscard]] int GetMousePositionY() const noexcept;
	void SetMousePositionY(int y_) noexcept;

	[[nodiscard]] std::pair<int, int> GetMousePosition() const noexcept;
	void SetMousePosition(int x_, int y_) noexcept;

	[[nodiscard]] std::pair<int, int> GetScreenSize() const noexcept;
	void SetScreenSize(int width_, int height_) noexcept;

private:
	std::array<bool, KeyCount> currentKeys{};
	std::array<bool, KeyCount> previousKeys{};

	std::array<bool, MouseButtonCount> currentMouseButtons{};
	std::array<bool, MouseButtonCount> previousMouseButtons{};

	int screenWidth{ 1 };
	int screenHeight{ 1 };

	int mouseX{ 0 };
	int mouseY{ 0 };

	std::vector<KeyCode> dispatchKeys;
};
