#pragma once

#include <chrono>
#include <format>
#include <print>
#include <string_view>

namespace Logger
{
	enum class Level
	{
		Trace,
		Info,
		Warning,
		Error,
		Critical
	};

	template <class... Args>
	void Log(Level level_, std::wstring_view format_, Args&&... args_)
	{
		std::wstring levelStr;
		switch (level_)
		{
		case Level::Trace:    levelStr = L"TRACE"; break;
		case Level::Info:     levelStr = L"INFO"; break;
		case Level::Warning:  levelStr = L"WARN"; break;
		case Level::Error:    levelStr = L"ERROR"; break;
		case Level::Critical: levelStr = L"CRIT"; break;
		}

		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::chrono::local_time<std::chrono::system_clock::duration> time = std::chrono::current_zone()->to_local(now);

		std::wstring message = std::vformat(format_, std::make_wformat_args(args_...));
		std::wcout << std::format(L"[{:%H:%M:%OS}] [{}] {}\n", time, levelStr, message);
	}

	template <class... Args>
	void Trace(std::wstring_view format_, Args&&... args_)
	{
		Log(Level::Trace, format_, std::forward<Args>(args_)...);
	}

	template <class... Args>
	void Info(std::wstring_view format_, Args&&... args_)
	{
		Log(Level::Info, format_, std::forward<Args>(args_)...);
	}

	template <class... Args>
	void Warning(std::wstring_view format_, Args&&... args_)
	{
		Log(Level::Warning, format_, std::forward<Args>(args_)...);
	}

	template <class... Args>
	void Error(std::wstring_view format_, Args&&... args_)
	{
		Log(Level::Error, format_, std::forward<Args>(args_)...);
	}

	template <class... Args>
	void Critical(std::wstring_view format_, Args&&... args_)
	{
		Log(Level::Critical, format_, std::forward<Args>(args_)...);
	}
}
