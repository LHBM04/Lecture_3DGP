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
	bool Initialize(const std::filesystem::path& filePath_ = "Logs/Engine.log");
	void Release();

	void SetMinimumLevel(LogLevel level_) noexcept;
	[[nodiscard]] LogLevel GetMinimumLevel() noexcept;

	void EnableConsoleOutput(bool enabled_) noexcept;
	void EnableDebugOutput(bool enabled_) noexcept;
	void EnableFileOutput(bool enabled_) noexcept;

	void Write(LogLevel level_, std::string_view message_);
	void Write(LogLevel level_, std::wstring_view message_);

	template <class... TArgs>
	void Log(LogLevel level_, std::format_string<TArgs...> format_, TArgs&&... args_)
	{
		Write(level_, std::format(format_, std::forward<TArgs>(args_)...));
	}

	template <class... TArgs>
	void Trace(std::format_string<TArgs...> format_, TArgs&&... args_)
	{
		Log(LogLevel::Trace, format_, std::forward<TArgs>(args_)...);
	}

	template <class... TArgs>
	void Info(std::format_string<TArgs...> format_, TArgs&&... args_)
	{
		Log(LogLevel::Info, format_, std::forward<TArgs>(args_)...);
	}

	template <class... TArgs>
	void Warning(std::format_string<TArgs...> format_, TArgs&&... args_)
	{
		Log(LogLevel::Warning, format_, std::forward<TArgs>(args_)...);
	}

	template <class... TArgs>
	void Error(std::format_string<TArgs...> format_, TArgs&&... args_)
	{
		Log(LogLevel::Error, format_, std::forward<TArgs>(args_)...);
	}

	template <class... TArgs>
	void Critical(std::format_string<TArgs...> format_, TArgs&&... args_)
	{
		Log(LogLevel::Critical, format_, std::forward<TArgs>(args_)...);
	}
}
