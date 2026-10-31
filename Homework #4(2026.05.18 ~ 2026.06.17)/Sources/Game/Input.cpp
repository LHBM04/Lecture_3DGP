#include "Precompiled.hpp"
#include "Input.hpp"

namespace TUK::Game
{
	namespace
	{
		Framework::InputProvider* provider = nullptr;
	}

	void Input::Startup(Framework::InputProvider& inputProvider)
	{
		provider = &inputProvider;
	}

	void Input::Shutdown()
	{
		provider = nullptr;
	}

	bool Input::IsKeyPressed(int key)
	{
		return provider && provider->IsKeyPressed(key);
	}

	bool Input::IsKeyHeld(int key)
	{
		return provider && provider->IsKeyHeld(key);
	}

	bool Input::IsKeyReleased(int key)
	{
		return provider && provider->IsKeyReleased(key);
	}
}
