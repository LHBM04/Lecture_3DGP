#pragma once

#include <unordered_map>

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <wrl.h>

#include <GameInput.h>

#include "../Core/AppSubsystem.hpp"
#include "../Core/Common.hpp"

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
		void OnActivation(const InputActivationEvent& event);

		[[nodiscard]] const InputProvider* GetProvider() const noexcept;
		[[nodiscard]] const InputProvider* GetProvider(const Display& display) const noexcept;

	private:
		void PollKeyboard(InputProvider& provider) noexcept;
		void PollMouse(InputProvider& provider) noexcept;

		ComPtr<IGameInput> gameInput;

		const Display* activeDisplay;
		
		std::unordered_map<const Display*, InputProvider> providers;

		bool hasMouseHistory;
		Vector2D<i32> lastMousePosition;
		Vector2D<i32> lastMouseWheel;
	};

	inline InputSubsystem::InputSubsystem() noexcept
		: AppSubsystem(0)
		, gameInput(nullptr)
		, activeDisplay(nullptr)
		, providers()
		, hasMouseHistory(false)
		, lastMousePosition()
		, lastMouseWheel()
	{
	}

	inline const InputProvider* InputSubsystem::GetProvider() const noexcept
	{
		return activeDisplay ? GetProvider(*activeDisplay) : nullptr;
	}

	inline const InputProvider* InputSubsystem::GetProvider(const Display& display) const noexcept
	{
		const auto it = providers.find(&display);
		return it != providers.end() ? &it->second : nullptr;
	}
}
