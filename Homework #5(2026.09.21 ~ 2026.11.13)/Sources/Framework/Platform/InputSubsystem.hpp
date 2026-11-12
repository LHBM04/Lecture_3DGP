#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include "../Core/AppSubsystem.hpp"
#include "../Core/Common.hpp"

#include "DisplayEvents.hpp"
#include "InputAction.hpp"
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

		[[nodiscard]] bool IsInputEnabled() const noexcept;
		void SetInputEnabled(bool value) noexcept;

		InputAction* AddAction(std::string_view name);
		void RemoveAction(std::string_view name) noexcept;
		[[nodiscard]] InputAction* GetAction(std::string_view name) noexcept;
		[[nodiscard]] const InputAction* GetAction(std::string_view name) const noexcept;

		void OnDisplayCreate(const DisplayCreateEvent& event);
		void OnDisplayClose(const DisplayCloseEvent& event);
		void OnDisplayFocus(const DisplayFocusEvent& event);
		void OnInputActivation(const InputActivationEvent& event);

		[[nodiscard]] const Display* GetActiveDisplay() const noexcept;
		[[nodiscard]] const InputProvider* GetProvider() const noexcept;

	private:
		void PollKeyboard(InputProvider& targetProvider) noexcept;
		void PollMouse(InputProvider& targetProvider) noexcept;
		void ResetInputState() noexcept;

		Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> gameInput;

		const Display* activeDisplay = nullptr;
		InputProvider provider{};
		std::unordered_map<std::string, std::unique_ptr<InputAction>> actions;

		bool isInputEnabled = true;
		bool needsInputSync = false;
		bool hasMouseHistory = false;
		Vector2D<i32> lastMousePosition{};
		Vector2D<i32> lastMouseWheel{};
	};

	inline InputSubsystem::InputSubsystem() noexcept
		: AppSubsystem(0)
	{
	}

	inline bool InputSubsystem::IsInputEnabled() const noexcept
	{
		return isInputEnabled;
	}

	inline const Display* InputSubsystem::GetActiveDisplay() const noexcept
	{
		return activeDisplay;
	}

	inline const InputProvider* InputSubsystem::GetProvider() const noexcept
	{
		return activeDisplay ? &provider : nullptr;
	}
}
