#pragma once

#include <unordered_map>

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "../Core/AppSubsystem.hpp"

#include "DisplayEvents.hpp"
#include "InputEvents.hpp"
#include "InputProvider.hpp"

namespace TUK::Framework
{
	class Display;

	class InputSubsystem : public AppSubsystem
	{
	public:
		InputSubsystem() noexcept;
		~InputSubsystem() noexcept override = default;

		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

		void AddProvider(const Display& display);
		void RemoveProvider(const Display& display);

		void OnCreate(const DisplayCreateEvent& event);
		void OnClose(const DisplayCloseEvent& event);
		void OnFocus(const DisplayFocusEvent& event);
		void OnKey(const KeyInterectEvent& event);
		void OnButton(const ButtonInterectEvent& event);
		void OnMouseMove(const MouseMoveEvent& event);
		void OnMouseWheel(const MouseWheelEvent& event);
		void OnMouseRawInput(const MouseRawInputEvent& event);
		void OnActivation(const InputActivationEvent& event);

		[[nodiscard]] const InputProvider* GetProvider(const Display& display) const noexcept;

	private:
		HWND mouseWindow = nullptr;
		const Display* activeDisplay = nullptr;
		std::unordered_map<const Display*, InputProvider> providers;
	};

	inline InputSubsystem::InputSubsystem() noexcept
		: AppSubsystem(0)
	{
	}

	inline const InputProvider* InputSubsystem::GetProvider(const Display& display) const noexcept
	{
		const auto it = providers.find(&display);
		return it != providers.end() ? &it->second : nullptr;
	}
}
