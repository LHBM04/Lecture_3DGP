#include "Precompiled.hpp"
#include "Input.hpp"

namespace TUK::Game
{
	namespace
	{
		InputSubsystem* subsystem = nullptr;

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
		subsystem = nullptr;
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
