#include "Precompiled.hpp"
#include "System.hpp"

#include "Memory.hpp"
#include "../../../../Homework #3(2026.05.18 ~ 2026.06.08)/Sources/Logger.h"

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
		Logger::Info(L"시스템 시작");

		std::ranges::sort(subsystems, std::ranges::less{}, &Subsystem::GetPriority);
		for (std::unique_ptr<Subsystem>& subsystem : subsystems)
		{
			if (subsystem->OnStartup() == SystemStatus::Failure)
			{
				return SystemStatus::Failure;
			}

			Logger::Trace(L"서브시스템 시작");
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
