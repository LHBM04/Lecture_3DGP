#include "Precompiled.hpp"
#include "Logger.hpp"

#include <format>
#include <iostream>
#include <source_location>
#include <string>
#include <string_view>
#include <syncstream>

#include <windows.h>

namespace TUK::Framework
{
	namespace
	{
		[[nodiscard]] std::string_view GetLevelName(LogLevel level) noexcept
		{
			switch (level)
			{
			case LogLevel::DEBUG:
				return "DEBUG";
			case LogLevel::INFO:
				return "INFO";
			case LogLevel::WARNING:
				return "WARNING";
			case LogLevel::ERROR_LEVEL:
				return "ERROR";
			default:
				return "UNKNOWN";
			}
		}
	}

	namespace Logger
	{
		void Log(LogLevel level, std::string_view message, std::source_location location)
		{
			const std::string line = std::format("[{}] {} ({}:{})\n", GetLevelName(level), message, location.file_name(), location.line());
			std::osyncstream(std::clog) << line << std::flush;
			OutputDebugStringA(line.c_str());
		}
	}
}
