#include "Precompiled.hpp"
#include "Input.hpp"

namespace TUK::Game
{
	namespace
	{
		const InputProvider* provider = nullptr;
	}

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

	Vector2D<i32> Input::GetMousePositionDelta() noexcept
	{
		return provider ? provider->GetMousePositionDelta() : Vector2D<i32>{};
	}
}
