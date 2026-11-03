#include "Precompiled.hpp"
#include "InputSubsystem.hpp"

#include "../Platform/Display.hpp"
#include "../Platform/InputProvider.hpp"

namespace TUK::Framework
{
	InputSubsystem::InputSubsystem() noexcept
		: AppSubsystem(0)
	{
	}

	InputSubsystem::~InputSubsystem() noexcept
	{
	}

	void InputSubsystem::AddProvider(const Display& display)
	{
		providers[&display] = std::make_unique<InputProvider>();
	}

	void InputSubsystem::RemoveProvider(const Display& display)
	{
		providers.erase(&display);
	}

	SystemStatus InputSubsystem::OnStartup()
	{
		return SystemStatus::Success;
	}

	SystemStatus InputSubsystem::OnEarlyUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus InputSubsystem::OnFixedUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus InputSubsystem::OnUpdate()
	{
		for (auto& provider : providers | std::views::values)
		{
			if (provider->IsKeyPressed(VK_SPACE))
			{
				std::cout << "Space key pressed!" << std::endl;
			}
			else if (provider->IsKeyHeld(VK_SPACE))
			{
				std::cout << "Space key held!" << std::endl;
			}
			else if (provider->IsKeyReleased(VK_SPACE))
			{
				std::cout << "Space key released!" << std::endl;
			}
		}

		return SystemStatus::Continue;
	}

	SystemStatus InputSubsystem::OnLateUpdate()
	{
		for (auto& provider : providers | std::views::values)
		{
			provider->Reset();
		}

		return SystemStatus::Continue;
	}

	void InputSubsystem::OnShutdown()
	{
		AppSubsystem::OnShutdown();
	}
}
