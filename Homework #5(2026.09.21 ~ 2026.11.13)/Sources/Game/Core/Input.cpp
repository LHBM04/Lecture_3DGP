#include "Precompiled.hpp"
#include "Input.hpp"

namespace
{
	const InputProvider* provider = nullptr;
}

namespace TUK::Game
{
	void Input::Initialize(const InputProvider* newProvider)
	{
		provider = newProvider;
	}

	void Input::Release()
	{
		provider = nullptr;
	}

	bool Input::IsKeyPressed(KeyCode keyCode)
	{
		return provider && provider->IsKeyPressed(keyCode);
	}

	bool Input::IsKeyHeld(KeyCode keyCode)
	{
		return provider && provider->IsKeyHeld(keyCode);
	}

	bool Input::IsKeyReleased(KeyCode keyCode)
	{
		return provider && provider->IsKeyReleased(keyCode);
	}

	bool Input::IsMouseButtonPressed(ButtonCode button)
	{
		return provider && provider->IsMouseButtonPressed(button);
	}

	bool Input::IsMouseButtonHeld(ButtonCode button)
	{
		return provider && provider->IsMouseButtonHeld(button);
	}

	bool Input::IsMouseButtonReleased(ButtonCode button)
	{
		return provider && provider->IsMouseButtonReleased(button);
	}

	float Input::GetHorizontal(KeyCode left, KeyCode right)
	{
		if (!provider)
		{
			return 0.0f;
		}
		return static_cast<f32>(Input::IsKeyHeld(left)) - static_cast<f32>(Input::IsKeyHeld(right));
	}

	float Input::GetVertical(KeyCode up, KeyCode Down)
	{
		if (!provider)
		{
			return 0.0f;
		}
		return static_cast<f32>(Input::IsKeyHeld(up)) - static_cast<f32>(Input::IsKeyHeld(Down));
	}

	Vector2D<i32> Input::GetMousePosition() noexcept
	{
		return provider ? provider->GetMousePosition() : Vector2D<i32>{};
	}

	Vector2D<i32> Input::GetMousePositionDelta() noexcept
	{
		return provider ? provider->GetMousePositionDelta() : Vector2D<i32>{};
	}
}
