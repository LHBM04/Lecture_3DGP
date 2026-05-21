#pragma once

#include "MouseButton.h"

class Input final
{
	STATIC_CLASS(Input);

public:
	static void BeginFrame() noexcept;

	[[nodiscard]] static bool IsKeyHeld(int virtualKey_) noexcept;
	[[nodiscard]] static bool IsKeyPressed(int virtualKey_) noexcept;
	[[nodiscard]] static bool IsKeyReleased(int virtualKey_) noexcept;

	[[nodiscard]] static bool IsMouseButtonDown(MouseButton button_) noexcept;
	[[nodiscard]] static bool IsMouseButtonPressed(MouseButton button_) noexcept;
	[[nodiscard]] static bool IsMouseButtonReleased(MouseButton button_) noexcept;

	[[nodiscard]] static int GetMouseX() noexcept;
	[[nodiscard]] static int GetMouseY() noexcept;
	[[nodiscard]] static int GetMouseDeltaX() noexcept;
	[[nodiscard]] static int GetMouseDeltaY() noexcept;

	static void SetKey(int virtualKey_, bool isDown_) noexcept;
	static void SetMouseButton(MouseButton button_, bool isDown_) noexcept;
	static void SetMousePosition(int x_, int y_) noexcept;

private:
	static constexpr std::size_t keyCount = 256;
	static constexpr std::size_t mouseButtonCount = static_cast<std::size_t>(MouseButton::Count);

	static inline std::array<bool, keyCount> currentKeys{};
	static inline std::array<bool, keyCount> previousKeys{};
	static inline std::array<bool, mouseButtonCount> currentMouseButtons{};
	static inline std::array<bool, mouseButtonCount> previousMouseButtons{};

	static inline int mouseX{ 0 };
	static inline int mouseY{ 0 };
	static inline int previousMouseX{ 0 };
	static inline int previousMouseY{ 0 };
};
