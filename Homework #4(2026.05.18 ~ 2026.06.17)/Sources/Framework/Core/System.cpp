#include "Precompiled.hpp"
#include "System.hpp"

namespace TUK::Framework
{
	SystemStatus System::Startup()
	{
		std::ranges::sort(subsystems, std::ranges::less{}, &Subsystem::GetPriority);

		for (const std::unique_ptr<Subsystem>& subsystem : subsystems)
		{
			if (subsystem->OnStartup() == SystemStatus::FAILURE)
			{
				return SystemStatus::FAILURE;
			}
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus System::Routine()
	{
		return SystemStatus::CONTINUE;
	}

	void System::Shutdown()
	{
		for (const std::unique_ptr<Subsystem>& subsystem : subsystems | std::views::reverse)
		{
			subsystem->OnShutdown();
		}

		subsystems.clear();
		subsystemsByType.clear();
	}
}
