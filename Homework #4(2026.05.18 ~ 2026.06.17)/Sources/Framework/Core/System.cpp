#include "Precompiled.hpp"
#include "System.hpp"

#include "Memory.hpp"

namespace TUK::Framework
{
	System::System() noexcept
		: subsystems(Memory::GetPersistentResource())
		, subsystemsByType(Memory::GetPersistentResource())
	{
		constexpr std::size_t DefaultCapacity = 16;

		subsystems.reserve(DefaultCapacity);
		subsystemsByType.reserve(DefaultCapacity);
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
			if (subsystem->OnStartup() == SystemStatus::Failure)
			{
				return SystemStatus::Failure;
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
