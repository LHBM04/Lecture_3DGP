#include "Precompiled.hpp"
#include "InputSubsystem.hpp"

#include "../Core/Common.hpp"

#include "Display.hpp"

namespace TUK::Framework
{
	using namespace GameInput::v3;

	SystemStatus InputSubsystem::OnStartup()
	{
		if (FAILED(GameInputCreate(&gameInput)))
		{
			return SystemStatus::FAILURE;
		}

		gameInput->SetFocusPolicy(GameInputDefaultFocusPolicy);
		return SystemStatus::CONTINUE;
	}

	SystemStatus InputSubsystem::OnEarlyUpdate()
	{
		if (!isInputEnabled)
		{
			return SystemStatus::CONTINUE;
		}

		const auto it = providers.find(activeDisplay);
		if (it == providers.end())
		{
			return SystemStatus::CONTINUE;
		}

		InputProvider& provider = it->second;
		if (GetForegroundWindow() != activeDisplay->GetHWND())
		{
			provider.Reset();
			hasMouseHistory = false;
			return SystemStatus::CONTINUE;
		}

		provider.Update();
		PollKeyboard(provider);
		PollMouse(provider);
		if (needsInputSync)
		{
			provider.SynchronizeHistory();
			needsInputSync = false;
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus InputSubsystem::OnFixedUpdate()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus InputSubsystem::OnUpdate()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus InputSubsystem::OnLateUpdate()
	{
		return SystemStatus::CONTINUE;
	}

	void InputSubsystem::OnShutdown()
	{
		gameInput.Reset();
		activeDisplay = nullptr;
		providers.clear();
		hasMouseHistory = false;
	}

	void InputSubsystem::SetInputEnabled(bool value) noexcept
	{
		if (isInputEnabled == value)
		{
			return;
		}

		isInputEnabled = value;
		needsInputSync = value;
		hasMouseHistory = false;
		for (InputProvider& provider : providers | std::views::values)
		{
			provider.Reset();
		}
	}

	void InputSubsystem::AddProvider(const Display* display)
	{
		ASSERT_FMT(display, "입력 제공자를 등록할 디스플레이가 nullptr입니다.");

		providers.try_emplace(display);
		if (display->GetHWND() == GetFocus())
		{
			activeDisplay = display;
			hasMouseHistory = false;
		}
	}

	void InputSubsystem::RemoveProvider(const Display* display)
	{
		ASSERT_FMT(display, "입력 제공자를 해제할 디스플레이가 nullptr입니다.");

		if (activeDisplay == display)
		{
			activeDisplay = nullptr;
			hasMouseHistory = false;
		}
		providers.erase(display);
	}

	void InputSubsystem::OnDisplayCreate(const DisplayCreateEvent& event)
	{
		AddProvider(event.display);
	}

	void InputSubsystem::OnDisplayClose(const DisplayCloseEvent& event)
	{
		RemoveProvider(event.display);
	}

	void InputSubsystem::OnDisplayFocus(const DisplayFocusEvent& event)
	{
		const auto it = providers.find(event.display);
		if (it == providers.end())
		{
			return;
		}

		it->second.Reset();
		hasMouseHistory = false;

		if (!event.isFocused)
		{
			if (activeDisplay == event.display)
			{
				activeDisplay = nullptr;
			}
			return;
		}

		if (const auto previous = providers.find(activeDisplay); previous != providers.end())
		{
			previous->second.Reset();
		}

		activeDisplay = event.display;
	}

	void InputSubsystem::OnInputActivation(const InputActivationEvent& event)
	{
		if (!event.isActive)
		{
			for (InputProvider& provider : providers | std::views::values)
			{
				provider.Reset();
			}
			hasMouseHistory = false;
		}
	}

	void InputSubsystem::PollKeyboard(InputProvider& provider) noexcept
	{
		Microsoft::WRL::ComPtr<IGameInputReading> reading;
		if (FAILED(gameInput->GetCurrentReading(GameInputKindKeyboard, nullptr, &reading)))
		{
			return;
		}

		const u32 KEY_COUNT = reading->GetKeyCount();
		if (KEY_COUNT == 0)
		{
			return;
		}

		std::array<GameInputKeyState, 32> keyStates{};
		const u32 READ_COUNT = reading->GetKeyState(
			std::min(KEY_COUNT, static_cast<u32>(keyStates.size())),
			keyStates.data());

		for (u32 i = 0; i < READ_COUNT; ++i)
		{
			const KeyCode KEY = static_cast<KeyCode>(keyStates[i].virtualKey);
			provider.SetKey(KEY, true);
		}

		provider.SetKey(
			KeyCode::SHIFT,
			provider.IsKeyHeld(KeyCode::LEFT_SHIFT) || provider.IsKeyHeld(KeyCode::RIGHT_SHIFT));
		provider.SetKey(
			KeyCode::CONTROL,
			provider.IsKeyHeld(KeyCode::LEFT_CONTROL) || provider.IsKeyHeld(KeyCode::RIGHT_CONTROL));
		provider.SetKey(
			KeyCode::ALT,
			provider.IsKeyHeld(KeyCode::LEFT_ALT) || provider.IsKeyHeld(KeyCode::RIGHT_ALT));
	}

	void InputSubsystem::PollMouse(InputProvider& provider) noexcept
	{
		POINT cursorPos{};
		if (GetCursorPos(&cursorPos) && ScreenToClient(activeDisplay->GetHWND(), &cursorPos))
		{
			provider.SetMousePosition({ cursorPos.x, cursorPos.y });
		}

		Microsoft::WRL::ComPtr<IGameInputReading> reading;
		if (FAILED(gameInput->GetCurrentReading(GameInputKindMouse, nullptr, &reading)))
		{
			hasMouseHistory = false;
			return;
		}

		GameInputMouseState mouseState{};
		if (!reading->GetMouseState(&mouseState))
		{
			return;
		}

		provider.SetMouseButtons(static_cast<u32>(mouseState.buttons));

		const Vector2D<i32> CURRENT_POS{
			static_cast<i32>(mouseState.positionX),
			static_cast<i32>(mouseState.positionY)
		};
		const Vector2D<i32> CURRENT_WHEEL{
			static_cast<i32>(mouseState.wheelX),
			static_cast<i32>(mouseState.wheelY)
		};

		if (hasMouseHistory)
		{
			const Vector2D<i32> POS_DELTA = CURRENT_POS - lastMousePosition;
			const Vector2D<i32> WHEEL_OFFSET = CURRENT_WHEEL - lastMouseWheel;

			provider.AddMousePositionDelta(POS_DELTA);
			provider.AddMouseWheelDelta(WHEEL_OFFSET);
		}

		lastMousePosition = CURRENT_POS;
		lastMouseWheel = CURRENT_WHEEL;
		hasMouseHistory = true;
	}
}
