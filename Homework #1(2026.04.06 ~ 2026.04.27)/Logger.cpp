#include "Precompiled.hpp"
#include "Logger.hpp"

void Logger::Trace(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Log("TRACE", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, format, args);
	va_end(args);
}

void Logger::Info(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Log("INFO", FOREGROUND_GREEN | FOREGROUND_INTENSITY, format, args);
	va_end(args);
}

void Logger::Warning(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Log("WARNING", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, format, args);
	va_end(args);
}

void Logger::Error(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Log("ERROR", FOREGROUND_RED | FOREGROUND_INTENSITY, format, args);
	va_end(args);
}

void Logger::Critical(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Log("CRITICAL", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY, format, args);
	va_end(args);
}

void Logger::Log(const char* level, WORD consoleColor, const char* format, va_list args)
{
	FILE* out = (strcmp(level, "ERROR") == 0 || strcmp(level, "CRITICAL") == 0) ? stderr : stdout;
	HANDLE consoleHandle = (out == stderr) ? GetStdHandle(STD_ERROR_HANDLE) : GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo{};
	const bool hasConsoleInfo = consoleHandle != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(consoleHandle, &consoleInfo);

	if (hasConsoleInfo)
	{
		SetConsoleTextAttribute(consoleHandle, consoleColor);
	}

	fprintf(out, "[%-8s] ", level);

	if (hasConsoleInfo)
	{
		SetConsoleTextAttribute(consoleHandle, consoleInfo.wAttributes);
	}

	vfprintf(out, format, args);
	fprintf(out, "\n");
	fflush(out);
}
