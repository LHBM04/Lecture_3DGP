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
		if (!isInputEnabled || !activeDisplay)
		{
			return SystemStatus::CONTINUE;
		}

		if (GetForegroundWindow() != activeDisplay->GetHWND())
		{
			ResetInputState();
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

		for (const auto& action : actions | std::views::values)
		{
			action->Evaluate(provider);
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
		ResetInputState();
		actions.clear();
		gameInput.Reset();
		activeDisplay = nullptr;
	}

	void InputSubsystem::SetInputEnabled(bool value) noexcept
	{
		if (isInputEnabled == value)
		{
			return;
		}

		isInputEnabled = value;
		needsInputSync = value;
		ResetInputState();
	}

	InputAction* InputSubsystem::AddAction(std::string_view name)
	{
		const std::string key(name);
		auto [it, inserted] = actions.try_emplace(key, nullptr);
		if (inserted)
		{
			it->second = std::make_unique<InputAction>(key);
		}
		return it->second.get();
	}

	void InputSubsystem::RemoveAction(std::string_view name) noexcept
	{
		const auto it = actions.find(std::string(name));
		if (it != actions.end())
		{
			it->second->Reset();
			actions.erase(it);
		}
	}

	InputAction* InputSubsystem::GetAction(std::string_view name) noexcept
	{
		const auto it = actions.find(std::string(name));
		return it != actions.end() ? it->second.get() : nullptr;
	}

	const InputAction* InputSubsystem::GetAction(std::string_view name) const noexcept
	{
		const auto it = actions.find(std::string(name));
		return it != actions.end() ? it->second.get() : nullptr;
	}

	void InputSubsystem::OnDisplayCreate(const DisplayCreateEvent& event)
	{
		ASSERT_FMT(event.display, "입력 서브시스템에 등록할 디스플레이가 nullptr입니다.");

		if (!activeDisplay || event.display->GetHWND() == GetFocus())
		{
			activeDisplay = event.display;
			hasMouseHistory = false;
		}
	}

	void InputSubsystem::OnDisplayClose(const DisplayCloseEvent& event)
	{
		if (activeDisplay == event.display)
		{
			ResetInputState();
			activeDisplay = nullptr;
		}
	}

	void InputSubsystem::OnDisplayFocus(const DisplayFocusEvent& event)
	{
		if (!event.display)
		{
			return;
		}

		ResetInputState();

		if (!event.isFocused)
		{
			if (activeDisplay == event.display)
			{
				activeDisplay = nullptr;
			}
			return;
		}

		activeDisplay = event.display;
	}

	void InputSubsystem::OnInputActivation(const InputActivationEvent& event)
	{
		if (!event.isActive)
		{
			ResetInputState();
		}
	}

	void InputSubsystem::PollKeyboard(InputProvider& targetProvider) noexcept
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
			targetProvider.SetKey(KEY, true);
		}

		targetProvider.SetKey(
			KeyCode::SHIFT,
			targetProvider.IsKeyHeld(KeyCode::LEFT_SHIFT) || targetProvider.IsKeyHeld(KeyCode::RIGHT_SHIFT));
		targetProvider.SetKey(
			KeyCode::CONTROL,
			targetProvider.IsKeyHeld(KeyCode::LEFT_CONTROL) || targetProvider.IsKeyHeld(KeyCode::RIGHT_CONTROL));
		targetProvider.SetKey(
			KeyCode::ALT,
			targetProvider.IsKeyHeld(KeyCode::LEFT_ALT) || targetProvider.IsKeyHeld(KeyCode::RIGHT_ALT));
	}

	void InputSubsystem::PollMouse(InputProvider& targetProvider) noexcept
	{
		POINT cursorPos{};
		if (GetCursorPos(&cursorPos) && ScreenToClient(activeDisplay->GetHWND(), &cursorPos))
		{
			targetProvider.SetMousePosition({ cursorPos.x, cursorPos.y });
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

		targetProvider.SetMouseButtons(static_cast<u32>(mouseState.buttons));

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

			targetProvider.AddMousePositionDelta(POS_DELTA);
			targetProvider.AddMouseWheelDelta(WHEEL_OFFSET);
		}

		lastMousePosition = CURRENT_POS;
		lastMouseWheel = CURRENT_WHEEL;
		hasMouseHistory = true;
	}

	void InputSubsystem::ResetInputState() noexcept
	{
		for (const auto& action : actions | std::views::values)
		{
			action->Reset();
		}
		provider.Reset();
		hasMouseHistory = false;
	}
}
