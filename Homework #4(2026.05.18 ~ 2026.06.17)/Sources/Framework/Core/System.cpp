#include "Precompiled.h"
#include "System.h"

namespace TUK::Framework
{
	System::System()
		: isRunning(false)
		, quitCode(EXIT_SUCCESS)
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

	void System::RequestQuit(int code) noexcept
	{
		if (quitCode == EXIT_SUCCESS)
		{
			quitCode = code;
		}
		isRunning = false;
	}

	void System::ReportError(std::string_view message)
	{
		const auto line = std::format("{}\n", message);
		OutputDebugStringA(line.c_str());
		RequestQuit(EXIT_FAILURE);
	}

	bool System::IsRunning() const noexcept
	{
		return isRunning;
	}

	void System::Startup()
	{
		isRunning = true;

		std::ranges::sort(subsystems, std::ranges::less{}, &Subsystem::GetPriority);
		for (std::unique_ptr<Subsystem>& subsystem : subsystems)
		{
			subsystem->OnStartup();
		}
	}

	void System::Shutdown()
	{
		isRunning = false;

		for (std::unique_ptr<Subsystem>& subsystem : subsystems | std::views::reverse)
		{
			subsystem->OnShutdown();
		}

		subsystems.clear();
		subsystemsByType.clear();
	}

	int System::GetQuitCode() const noexcept
	{
		return quitCode;
	}
}
