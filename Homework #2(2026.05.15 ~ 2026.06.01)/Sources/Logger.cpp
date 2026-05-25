#include "Precompiled.h"
#include "Logger.h"

#include <array>
#include <cstdarg>
#include <cstdio>
#include <mutex>

namespace
{
	const char* GetLogPrefix(LogLevel level_) noexcept
	{
		switch (level_)
		{
		case LogLevel::Trace:
			return "Trace";
		case LogLevel::Info:
			return "Info";
		case LogLevel::Warning:
			return "Warning";
		case LogLevel::Error:
			return "Error";
		case LogLevel::Critical:
			return "Critical";
		default:
			return "Unknown";
		}
	}

	std::string GetTimestamp()
	{
		SYSTEMTIME time{};
		::GetLocalTime(&time);

		return std::format(
			"{:04}-{:02}-{:02} {:02}:{:02}:{:02}.{:03}",
			time.wYear,
			time.wMonth,
			time.wDay,
			time.wHour,
			time.wMinute,
			time.wSecond,
			time.wMilliseconds);
	}

	std::string CreateFormat(const char* format_, va_list args_)
	{
		if (nullptr == format_)
		{
			return {};
		}

		std::array<char, 2048> buffer{};
		va_list copied{};
		va_copy(copied, args_);
		const int written{ std::vsnprintf(buffer.data(), buffer.size(), format_, copied) };
		va_end(copied);

		if (written <= 0)
		{
			return {};
		}

		const std::size_t count{
			static_cast<std::size_t>(std::min<int>(written, static_cast<int>(buffer.size() - 1)))
		};
		return std::string(buffer.data(), count);
	}
}

void Logger::Trace(const char* format_, ...)
{
	va_list args{};
	va_start(args, format_);
	Log(LogLevel::Trace, format_, args);
	va_end(args);
}

void Logger::Info(const char* format_, ...)
{
	va_list args{};
	va_start(args, format_);
	Log(LogLevel::Info, format_, args);
	va_end(args);
}

void Logger::Warning(const char* format_, ...)
{
	va_list args{};
	va_start(args, format_);
	Log(LogLevel::Warning, format_, args);
	va_end(args);
}

void Logger::Error(const char* format_, ...)
{
	va_list args{};
	va_start(args, format_);
	Log(LogLevel::Error, format_, args);
	va_end(args);
}

void Logger::Critical(const char* format_, ...)
{
	va_list args{};
	va_start(args, format_);
	Log(LogLevel::Critical, format_, args);
	va_end(args);
}

void Logger::Log(LogLevel level_, const char* format_, va_list args_)
{
	const std::string line{
		std::format("[{}][{}] {}\n", GetTimestamp(), GetLogPrefix(level_), CreateFormat(format_, args_))
	};

	::OutputDebugStringA(line.c_str());
	std::fputs(line.c_str(), stdout);
	std::fflush(stdout);
}
