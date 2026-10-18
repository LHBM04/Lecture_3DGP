#include "Precompiled.hpp"
#include "System.hpp"

namespace TUK::Framework
{
	System::System() noexcept
		:  subsystems()
		, subsystemsByType()
	{
	}

	System::~System() noexcept
	{
		subsystemsByType.clear();
		subsystems.clear();
	}

	SystemStatus System::Startup()
	{
		std::ranges::sort(subsystems, std::ranges::less{}, &Subsystem::GetPriority);
		for (std::unique_ptr<Subsystem>& subsystem : subsystems)
		{
			if (subsystem->OnStartup() == SystemStatus::Failed)
			{
				return SystemStatus::Failed;
			}
		}

		return SystemStatus::Continue;
	}

	SystemStatus System::Routine()
	{
		return SystemStatus::Continue;
	}

	void System::Shutdown()
	{
		for (std::unique_ptr<Subsystem>& subsystem : subsystems | std::views::reverse)
		{
			subsystem->OnShutdown();
		}

		subsystems.clear();
		subsystemsByType.clear();
	}
}
