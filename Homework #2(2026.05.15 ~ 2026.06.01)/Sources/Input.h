#pragma once

#include "Singleton.h"
#include "Event.h"
#include "KeyCode.h"
#include "MouseCode.h"

#include <array>

class Input final : public Singleton<Input>
{
public:
	bool Initialize() noexcept;
	void Release() noexcept;

	void Update() noexcept;
	void ProcessEvent(const Event& event_) noexcept;
	void EndFrame() noexcept;

	void SetScreenSize(int width_, int height_) noexcept;

	[[nodiscard]] bool IsKeyDown(KeyCode keyCode_) const noexcept;
	[[nodiscard]] bool IsKeyPressed(KeyCode keyCode_) const noexcept;
	[[nodiscard]] bool IsKeyReleased(KeyCode keyCode_) const noexcept;

	[[nodiscard]] bool IsMouseButtonDown(MouseCode mouseCode_) const noexcept;
	[[nodiscard]] bool IsMouseButtonPressed(MouseCode mouseCode_) const noexcept;
	[[nodiscard]] bool IsMouseButtonReleased(MouseCode mouseCode_) const noexcept;

	[[nodiscard]] int GetMouseX() const noexcept;
	[[nodiscard]] int GetMouseY() const noexcept;

private:
	bool isInitialized{ false };
	int screenWidth{ 1 };
	int screenHeight{ 1 };
	std::array<bool, 256> currentKeys{};
	std::array<bool, 256> previousKeys{};
	std::array<bool, 5> currentMouseButtons{};
	std::array<bool, 5> previousMouseButtons{};
	int mouseX{ 0 };
	int mouseY{ 0 };
};
