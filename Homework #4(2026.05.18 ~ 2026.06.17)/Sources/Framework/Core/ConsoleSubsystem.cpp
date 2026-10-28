#include "Precompiled.hpp"
#include "ConsoleSubsystem.hpp"

#include "Logger.hpp"

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
		if (!AllocConsole())
		{
			return SystemStatus::Failure;
		}

		// /utf-8로 컴파일한 문자열을 콘솔에서도 UTF-8로 해석합니다.
		if (!SetConsoleOutputCP(CP_UTF8))
		{
			return SystemStatus::Failure;
		}

		FILE* fp = nullptr;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);

		return SystemStatus::Continue;
	}

	void ConsoleSubsystem::OnShutdown()
	{
		FreeConsole();
	}
}
