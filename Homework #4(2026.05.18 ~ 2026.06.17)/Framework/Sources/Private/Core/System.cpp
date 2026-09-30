#include "Precompiled.h"
#include "Framework/Core/System.h"

#include "Framework/Core/Subsystem.h"

namespace TUK::Framework
{
	System::System() noexcept
		: isRunning(false)
	{
		
	}

	void System::Startup()
	{
		instance = std::unique_ptr<System>(this);

		for (std::unique_ptr<Subsystem>& subsystem : subsystems)
		{
			subsystem->OnStartup();
		}

		isRunning = true;
	}

	void System::Shutdown()
	{
		isRunning = false;

		for (std::unique_ptr<Subsystem>& subsystem : subsystems | std::views::reverse)
		{
			subsystem->OnShutdown();
		}

		instance.reset();
	}

	void System::Run()
	{
		while (isRunning)
		{
			for (const std::function<void()>& preTickableSubsystem : preTickableSubsystems)
			{
				preTickableSubsystem();
			}
			for (const std::function<void()>& tickableSubsystem : tickableSubsystems)
			{
				tickableSubsystem();
			}
			for (const std::function<void()>& postTickableSubsystem : postTickableSubsystems)
			{
				postTickableSubsystem();
			}
		}
	}

	System& System::GetInstance() noexcept
	{
		return *instance;
	}

	bool System::IsRunning() const noexcept
	{
		return isRunning;
	}

	void System::RequestQuit() noexcept
	{
		isRunning = false;
	}

	std::unique_ptr<System> System::instance = nullptr;
}
