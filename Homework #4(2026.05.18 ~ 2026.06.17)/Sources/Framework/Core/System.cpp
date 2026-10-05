#include "Precompiled.h"
#include "System.h"

#include <cstdlib>

namespace TUK::Framework
{
	System::System()
		: isRunning(false)
		, exitCode(EXIT_SUCCESS)
		, options()
		, subsystems()
		, subsystemsByType()
	{
		
	}

	System::~System()
	{
		isRunning = false;
		subsystemsByType.clear();
		subsystems.clear();
	}

	System& System::GetInstance()
	{
		return *instance;
	}

	int System::Run()
	{
		exitCode = EXIT_SUCCESS;

		/** 서브시스템 가동 */
		Startup();

		while (true)
		{
			for (auto& subsystem : subsystems)
			{
				subsystem->OnPreTick();
			}

			for (auto& subsystem : subsystems)
			{
				subsystem->OnTick();
			}

			for (auto& subsystem : subsystems)
			{
				subsystem->OnPostTick();
			}

			if (!isRunning)
			{
				break;
			}
		}

		/** 서브시스템 종료 */
		Shutdown();

		return exitCode;
	}

	void System::RequestQuit(int code) noexcept
	{
		exitCode = code;
		isRunning = false;
	}

	void System::Startup()
	{
		instance = this;

		isRunning = true;

		std::ranges::sort(subsystems, std::ranges::less{}, &Subsystem::GetPriority);
		for (auto& subsystem : subsystems)
		{
			subsystem->OnStartup();
		}
	}

	void System::Shutdown()
	{
		isRunning = false;

		for (auto& subsystem : subsystems | std::views::reverse)
		{
			subsystem->OnShutdown();
		}

		instance = nullptr;
	}

	System* System::instance = nullptr;
}
