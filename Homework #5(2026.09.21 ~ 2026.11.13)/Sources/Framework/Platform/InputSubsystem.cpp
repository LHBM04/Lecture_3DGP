#include "Precompiled.hpp"
#include "InputSubsystem.hpp"

#include "../Core/Engine.hpp"

#include "Display.hpp"
#include "EventSubsystem.hpp"

namespace TUK::Framework
{
	SystemStatus InputSubsystem::OnStartup()
	{
		providers.clear();
		activeDisplay = nullptr;
		mouseWindow = nullptr;

		const RAWINPUTDEVICE MOUSE{ 0x01, 0x02, 0, nullptr };
		if (!RegisterRawInputDevices(&MOUSE, 1, sizeof(MOUSE)))
		{
			return SystemStatus::FAILURE;
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus InputSubsystem::OnEarlyUpdate()
	{
		const auto it = providers.find(activeDisplay);
		if (it == providers.end())
		{
			return SystemStatus::CONTINUE;
		}

		InputProvider& provider = it->second;
		if (!mouseWindow || !IsWindow(mouseWindow) || GetForegroundWindow() != mouseWindow)
		{
			provider.Reset();
			return SystemStatus::CONTINUE;
		}

		if (EventSubsystem* events = Engine::GetInstance().GetSubsystem<EventSubsystem>())
		{
			for (u32 code = 0; code <= 0xFEu; ++code)
			{
				const KeyCode KEY = static_cast<KeyCode>(code);
				if (provider.IsKeyHeld(KEY))
				{
					events->DispatchEvent(KeyInterectEvent{ { *activeDisplay }, KEY, InputAction::HELD });
				}
			}
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
		for (InputProvider& provider : providers | std::views::values)
		{
			provider.Update();
		}

		return SystemStatus::CONTINUE;
	}

	void InputSubsystem::OnShutdown()
	{
		const RAWINPUTDEVICE MOUSE{ 0x01, 0x02, RIDEV_REMOVE, nullptr };
		RegisterRawInputDevices(&MOUSE, 1, sizeof(MOUSE));

		mouseWindow = nullptr;
		activeDisplay = nullptr;
		providers.clear();
	}

	void InputSubsystem::AddProvider(const Display& display)
	{
		providers.try_emplace(&display);
		if (display.GetHWND() == GetFocus())
		{
			activeDisplay = &display;
			mouseWindow = display.GetHWND();
		}
	}

	void InputSubsystem::RemoveProvider(const Display& display)
	{
		if (activeDisplay == &display)
		{
			activeDisplay = nullptr;
			mouseWindow = nullptr;
		}
		providers.erase(&display);
	}

	void InputSubsystem::OnCreate(const DisplayCreateEvent& event)
	{
		AddProvider(event.display);
	}

	void InputSubsystem::OnClose(const DisplayCloseEvent& event)
	{
		RemoveProvider(event.display);
	}

	void InputSubsystem::OnFocus(const DisplayFocusEvent& event)
	{
		const auto it = providers.find(&event.display);
		if (it == providers.end())
		{
			return;
		}

		it->second.Reset();
		if (!event.isFocused)
		{
			if (activeDisplay == &event.display)
			{
				activeDisplay = nullptr;
				mouseWindow = nullptr;
			}
			return;
		}

		if (const auto previous = providers.find(activeDisplay); previous != providers.end())
		{
			previous->second.Reset();
		}

		activeDisplay = &event.display;
		mouseWindow = event.display.GetHWND();
	}

	void InputSubsystem::OnKey(const KeyInterectEvent& event)
	{
		const auto it = providers.find(&event.display);
		if (it == providers.end())
		{
			return;
		}

		InputProvider& provider = it->second;
		const bool IS_DOWN = event.action != InputAction::RELEASED;
		provider.SetKey(event.key, IS_DOWN);

		if (event.key == KeyCode::LEFT_SHIFT || event.key == KeyCode::RIGHT_SHIFT)
		{
			provider.SetKey(KeyCode::SHIFT, provider.IsKeyHeld(KeyCode::LEFT_SHIFT) || provider.IsKeyHeld(KeyCode::RIGHT_SHIFT));
		}
		else if (event.key == KeyCode::LEFT_CONTROL || event.key == KeyCode::RIGHT_CONTROL)
		{
			provider.SetKey(KeyCode::CONTROL, provider.IsKeyHeld(KeyCode::LEFT_CONTROL) || provider.IsKeyHeld(KeyCode::RIGHT_CONTROL));
		}
		else if (event.key == KeyCode::LEFT_ALT || event.key == KeyCode::RIGHT_ALT)
		{
			provider.SetKey(KeyCode::ALT, provider.IsKeyHeld(KeyCode::LEFT_ALT) || provider.IsKeyHeld(KeyCode::RIGHT_ALT));
		}
	}

	void InputSubsystem::OnButton(const ButtonInterectEvent& event)
	{
		const auto it = providers.find(&event.display);
		if (it == providers.end())
		{
			return;
		}

		it->second.SetMouseButton(event.button, event.action == InputAction::PRESSED);
	}

	void InputSubsystem::OnMouseMove(const MouseMoveEvent& event)
	{
		const auto it = providers.find(&event.display);
		if (it == providers.end())
		{
			return;
		}

		it->second.SetMousePosition(event.position);
	}

	void InputSubsystem::OnMouseWheel(const MouseWheelEvent& event)
	{
		const auto it = providers.find(&event.display);
		if (it == providers.end())
		{
			return;
		}

		it->second.AddMouseWheelDelta(event.wheelDelta);
	}

	void InputSubsystem::OnMouseRawInput(const MouseRawInputEvent& event)
	{
		const auto it = providers.find(&event.display);
		if (it == providers.end())
		{
			return;
		}

		it->second.AddMousePositionDelta(event.delta);
	}

	void InputSubsystem::OnActivation(const InputActivationEvent& event)
	{
		if (!event.isActive)
		{
			for (InputProvider& provider : providers | std::views::values)
			{
				provider.Reset();
			}
		}
	}
}
