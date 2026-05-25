#pragma once

#include <filesystem>
#include <format>
#include <string>
#include <string_view>

enum class LogLevel : unsigned char
{
	Trace,
	Info,
	Warning,
	Error,
	Critical
};

namespace Logger
{
	void Trace(const char* format_, ...);
	void Info(const char* format_, ...);
	void Warning(const char* format_, ...);
	void Error(const char* format_, ...);
	void Critical(const char* format_, ...);

	void Log(LogLevel level_, const char* format_, va_list args_);
}
