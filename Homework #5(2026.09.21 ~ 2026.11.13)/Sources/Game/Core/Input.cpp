#include "Precompiled.hpp"
#include "Input.hpp"

#include "Framework/Core.hpp"

namespace TUK::Game
{
	namespace
	{
		InputSubsystem* subsystem = nullptr;
		bool cursorClipped = false;
		bool cursorBlockEnabled = false;
		D3D12_RECT cursorBoundary{};
		i32 cursorHideCalls = 0;

		[[nodiscard]] const InputProvider* GetActiveProvider() noexcept
		{
			return subsystem ? subsystem->GetProvider() : nullptr;
		}
	}

	void Input::Initialize(InputSubsystem* newSubsystem) noexcept
	{
		subsystem = newSubsystem;
	}

	void Input::Release() noexcept
	{
		EnableCursorBlock(false);
		subsystem = nullptr;
	}

	void Input::EnableCursorBlock(bool enabled) noexcept
	{
		cursorBlockEnabled = enabled;
		UpdateCursorClip();
	}

	void Input::SetCursorBoundary(const D3D12_RECT& boundary) noexcept
	{
		cursorBoundary = boundary;
		if (cursorBlockEnabled)
		{
			UpdateCursorClip();
		}
	}

	void Input::UpdateCursorClip() noexcept
	{
		const DisplaySubsystem* displays = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
		const Display* display = displays && displays->GetDisplayCount() > 0 ? displays->GetDisplay(0) : nullptr;
		const HWND window = display ? display->GetHWND() : nullptr;

		POINT topLeft{ cursorBoundary.left, cursorBoundary.top };
		POINT bottomRight{ cursorBoundary.right, cursorBoundary.bottom };

		if (cursorBlockEnabled && window && GetForegroundWindow() == window && !IsIconic(window)
			&& cursorBoundary.right > cursorBoundary.left && cursorBoundary.bottom > cursorBoundary.top
			&& ClientToScreen(window, &topLeft) && ClientToScreen(window, &bottomRight))
		{
			const RECT rect{ topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
			if (ClipCursor(&rect))
			{
				cursorClipped = true;
				SetCursorVisible(false);
				return;
			}
		}

		if (cursorClipped)
		{
			ClipCursor(nullptr);
			cursorClipped = false;
		}

		SetCursorVisible(true);
	}

	void Input::SetCursorVisible(bool visible) noexcept
	{
		if (visible)
		{
			while (cursorHideCalls > 0)
			{
				ShowCursor(TRUE);
				--cursorHideCalls;
			}
		}
		else if (cursorHideCalls == 0)
		{
			i32 count = 0;
			do
			{
				count = ShowCursor(FALSE);
				++cursorHideCalls;
			} while (count >= 0);
		}
	}

	InputAction* Input::AddAction(std::string_view name)
	{
		return subsystem ? subsystem->AddAction(name) : nullptr;
	}

	void Input::RemoveAction(std::string_view name) noexcept
	{
		if (subsystem)
		{
			subsystem->RemoveAction(name);
		}
	}

	InputAction* Input::GetAction(std::string_view name) noexcept
	{
		return subsystem ? subsystem->GetAction(name) : nullptr;
	}

	bool Input::IsKeyPressed(KeyCode keyCode) noexcept
	{
		const InputProvider* provider = GetActiveProvider();
		return provider && provider->IsKeyPressed(keyCode);
	}

	bool Input::IsKeyHeld(KeyCode keyCode) noexcept
	{
		const InputProvider* provider = GetActiveProvider();
		return provider && provider->IsKeyHeld(keyCode);
	}

	bool Input::IsKeyReleased(KeyCode keyCode) noexcept
	{
		const InputProvider* provider = GetActiveProvider();
		return provider && provider->IsKeyReleased(keyCode);
	}

	bool Input::IsMouseButtonPressed(ButtonCode button) noexcept
	{
		const InputProvider* provider = GetActiveProvider();
		return provider && provider->IsMouseButtonPressed(button);
	}

	bool Input::IsMouseButtonHeld(ButtonCode button) noexcept
	{
		const InputProvider* provider = GetActiveProvider();
		return provider && provider->IsMouseButtonHeld(button);
	}

	bool Input::IsMouseButtonReleased(ButtonCode button) noexcept
	{
		const InputProvider* provider = GetActiveProvider();
		return provider && provider->IsMouseButtonReleased(button);
	}

	f32 Input::GetHorizontal(KeyCode left, KeyCode right) noexcept
	{
		if (!GetActiveProvider())
		{
			return 0.0f;
		}
		return static_cast<f32>(Input::IsKeyHeld(left)) - static_cast<f32>(Input::IsKeyHeld(right));
	}

	f32 Input::GetVertical(KeyCode up, KeyCode down) noexcept
	{
		if (!GetActiveProvider())
		{
			return 0.0f;
		}
		return static_cast<f32>(Input::IsKeyHeld(up)) - static_cast<f32>(Input::IsKeyHeld(down));
	}

	Vector2D<i32> Input::GetMousePosition() noexcept
	{
		const InputProvider* provider = GetActiveProvider();
		return provider ? provider->GetMousePosition() : Vector2D<i32>{};
	}

	Vector2D<i32> Input::GetMousePositionDelta() noexcept
	{
		const InputProvider* provider = GetActiveProvider();
		return provider ? provider->GetMousePositionDelta() : Vector2D<i32>{};
	}
}
