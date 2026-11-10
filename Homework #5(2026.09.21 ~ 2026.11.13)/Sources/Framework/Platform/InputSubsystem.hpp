#pragma once

#include <unordered_map>

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <wrl.h>

#include <gameinput/gameinput.h>

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

		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

		void AddProvider(const Display* display);
		void RemoveProvider(const Display* display);

		void OnDisplayCreate(const DisplayCreateEvent& event);
		void OnDisplayClose(const DisplayCloseEvent& event);
		void OnDisplayFocus(const DisplayFocusEvent& event);
		void OnInputActivation(const InputActivationEvent& event);

		[[nodiscard]] const InputProvider* GetProvider() const noexcept;
		[[nodiscard]] const InputProvider* GetProvider(const Display* display) const noexcept;

	private:
		void PollKeyboard(InputProvider& provider) noexcept;
		void PollMouse(InputProvider& provider) noexcept;

		Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> gameInput;

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
		return activeDisplay ? GetProvider(activeDisplay) : nullptr;
	}

	inline const InputProvider* InputSubsystem::GetProvider(const Display* display) const noexcept
	{
		if (!display)
		{
			return nullptr;
		}

		const auto it = providers.find(display);
		return it != providers.end() ? &it->second : nullptr;
	}
}
