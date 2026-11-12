#pragma once

#include <bitset>

#include "../Core/Common.hpp"

#include "../Math/Vector2D.hpp"

#include "InputStates.hpp"

namespace TUK::Framework
{
	class InputProvider
	{
	public:
		InputProvider() noexcept = default;
		virtual ~InputProvider() noexcept = default;

		InputProvider(const InputProvider&) = delete;
		InputProvider& operator=(const InputProvider&) = delete;

		InputProvider(InputProvider&&) noexcept = default;
		InputProvider& operator=(InputProvider&&) noexcept = default;

		void Reset() noexcept;
		void Update() noexcept;
		void SynchronizeHistory() noexcept;

		void SetKey(KeyCode key, bool isDown) noexcept;
		void SetMouseButton(ButtonCode button, bool isDown) noexcept;
		void SetMouseButtons(u32 buttons) noexcept;
		void SetMousePosition(const Vector2D<i32>& position) noexcept;
		void AddMousePositionDelta(const Vector2D<i32>& positionDelta) noexcept;
		void AddMouseWheelDelta(const Vector2D<i32>& wheelDelta) noexcept;

		[[nodiscard]] bool IsKeyPressed(KeyCode key) const noexcept;
		[[nodiscard]] bool IsKeyHeld(KeyCode key) const noexcept;
		[[nodiscard]] bool IsKeyReleased(KeyCode key) const noexcept;

		[[nodiscard]] bool HasMouseButtonState() const noexcept;
		[[nodiscard]] bool IsMouseButtonPressed(ButtonCode button) const noexcept;
		[[nodiscard]] bool IsMouseButtonHeld(ButtonCode button) const noexcept;
		[[nodiscard]] bool IsMouseButtonReleased(ButtonCode button) const noexcept;

		[[nodiscard]] bool HasMousePosition() const noexcept;
		[[nodiscard]] Vector2D<i32> GetMousePosition() const noexcept;
		[[nodiscard]] Vector2D<i32> GetMousePositionDelta() const noexcept;
		[[nodiscard]] Vector2D<i32> GetMouseWheelDelta() const noexcept;

	private:
		std::bitset<256> currentkeys{};
		std::bitset<256> previousKeys{};
	
		u32 currentButtons = 0;
		u32 previousButtons = 0;
		bool hasMouseButtonState = false;

		bool hasMousePosition = false;
		Vector2D<i32> mousePosition{};
		Vector2D<i32> mousePositionDelta{};
		Vector2D<i32> mouseWheelDelta{};
	};

	inline void InputProvider::Reset() noexcept
	{
		currentkeys.reset();
		previousKeys.reset();

		currentButtons = 0;
		previousButtons = 0;
		hasMouseButtonState = false;

		hasMousePosition = false;
		mousePosition = {};
		mousePositionDelta = {};
		mouseWheelDelta = {};
	}

	inline void InputProvider::Update() noexcept
	{
		previousKeys = currentkeys;
		previousButtons = currentButtons;

		currentkeys.reset();
		currentButtons = 0;
		hasMouseButtonState = false;
		hasMousePosition = false;

		mousePositionDelta = {};
		mouseWheelDelta = {};
	}

	inline void InputProvider::SynchronizeHistory() noexcept
	{
		previousKeys = currentkeys;
		previousButtons = currentButtons;
		mousePositionDelta = {};
		mouseWheelDelta = {};
	}

	inline void InputProvider::SetKey(KeyCode key, bool isDown) noexcept
	{
		const usize INDEX = std::to_underlying(key);
		if (INDEX < currentkeys.size())
		{
			currentkeys.set(INDEX, isDown);
		}
	}

	inline void InputProvider::SetMouseButton(ButtonCode button, bool isDown) noexcept
	{
		hasMouseButtonState = true;
		const u32 MASK = std::to_underlying(button);
		if (isDown)
		{
			currentButtons |= MASK;
		}
		else
		{
			currentButtons &= ~MASK;
		}
	}

	inline void InputProvider::SetMouseButtons(u32 buttons) noexcept
	{
		currentButtons = buttons;
		hasMouseButtonState = true;
	}

	inline void InputProvider::SetMousePosition(const Vector2D<i32>& position) noexcept
	{
		mousePosition = position;
		hasMousePosition = true;
	}

	inline void InputProvider::AddMousePositionDelta(const Vector2D<i32>& positionDelta) noexcept
	{
		mousePositionDelta += positionDelta;
	}

	inline void InputProvider::AddMouseWheelDelta(const Vector2D<i32>& wheelDelta) noexcept
	{
		mouseWheelDelta += wheelDelta;
	}

	inline bool InputProvider::IsKeyPressed(KeyCode key) const noexcept
	{
		const usize INDEX = std::to_underlying(key);
		return INDEX < currentkeys.size() && currentkeys[INDEX] && !previousKeys[INDEX];
	}

	inline bool InputProvider::IsKeyHeld(KeyCode key) const noexcept
	{
		const usize INDEX = std::to_underlying(key);
		return INDEX < currentkeys.size() && currentkeys[INDEX];
	}

	inline bool InputProvider::IsKeyReleased(KeyCode key) const noexcept
	{
		const usize INDEX = std::to_underlying(key);
		return INDEX < currentkeys.size() && !currentkeys[INDEX] && previousKeys[INDEX];
	}

	inline bool InputProvider::HasMouseButtonState() const noexcept
	{
		return hasMouseButtonState;
	}

	inline bool InputProvider::IsMouseButtonPressed(ButtonCode button) const noexcept
	{
		const u32 MASK = std::to_underlying(button);
		return (currentButtons & MASK) != 0 && (previousButtons & MASK) == 0;
	}

	inline bool InputProvider::IsMouseButtonHeld(ButtonCode button) const noexcept
	{
		const u32 MASK = std::to_underlying(button);
		return (currentButtons & MASK) != 0;
	}

	inline bool InputProvider::IsMouseButtonReleased(ButtonCode button) const noexcept
	{
		const u32 MASK = std::to_underlying(button);
		return (currentButtons & MASK) == 0 && (previousButtons & MASK) != 0;
	}

	inline bool InputProvider::HasMousePosition() const noexcept
	{
		return hasMousePosition;
	}

	inline Vector2D<i32> InputProvider::GetMousePosition() const noexcept
	{
		return mousePosition;
	}

	inline Vector2D<i32> InputProvider::GetMousePositionDelta() const noexcept
	{
		return mousePositionDelta;
	}

	inline Vector2D<i32> InputProvider::GetMouseWheelDelta() const noexcept
	{
		return mouseWheelDelta;
	}
}
