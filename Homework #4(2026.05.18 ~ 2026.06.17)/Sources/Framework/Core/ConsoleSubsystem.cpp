#include "Precompiled.hpp"
#include "ConsoleSubsystem.hpp"

namespace TUK::Framework
{
	ConsoleSubsystem::ConsoleSubsystem() noexcept
		: EngineSubsystem(1)
	{
	}

	ConsoleSubsystem::~ConsoleSubsystem() noexcept
	{
		FreeConsole();
	}

	SystemStatus ConsoleSubsystem::OnStartup()
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		if (!AllocConsole())
		{
			return SystemStatus::Failure;
		}

		FILE* fp = nullptr;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);

		std::print(fp, "Debug console initialized.\n");

		return SystemStatus::Continue;
	}

	void ConsoleSubsystem::OnShutdown()
	{
		FreeConsole();
	}
}
