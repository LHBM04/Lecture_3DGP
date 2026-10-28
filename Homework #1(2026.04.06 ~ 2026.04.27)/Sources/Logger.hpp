#pragma once

class Logger
{
	STATIC_CLASS(Logger)
public:
	static void Trace(const char* format, ...);		
	static void Info(const char* format, ...);		
	static void Warning(const char* format, ...);	
	static void Error(const char* format, ...);		
	static void Critical(const char* format, ...);	

private:
	static void Log(const char* level, WORD consoleColor, const char* format, va_list args);
};
