#include "Precompiled.h"
#include "Logger.h"

#include <mutex>

namespace
{
	std::mutex mutex;
	std::ofstream fileStream;

	LogLevel minimumLevel{ LogLevel::Trace };
	bool consoleOutputEnabled{ true };
	bool debugOutputEnabled{ true };
	bool fileOutputEnabled{ true };

	const char* ToString(LogLevel level_) noexcept
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

	bool ShouldWrite(LogLevel level_) noexcept
	{
		return static_cast<unsigned char>(level_) >= static_cast<unsigned char>(minimumLevel);
	}

	std::string GetTimestamp()
	{
		SYSTEMTIME time;
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

	std::string WideToUtf8(std::wstring_view text_)
	{
		if (text_.empty())
		{
			return {};
		}

		const int requiredSize
		{
			::WideCharToMultiByte(
				CP_UTF8,
				0,
				text_.data(),
				static_cast<int>(text_.size()),
				nullptr,
				0,
				nullptr,
				nullptr)
		};

		if (requiredSize <= 0)
		{
			return {};
		}

		std::string result(static_cast<std::size_t>(requiredSize), '\0');
		::WideCharToMultiByte(
			CP_UTF8,
			0,
			text_.data(),
			static_cast<int>(text_.size()),
			result.data(),
			requiredSize,
			nullptr,
			nullptr);

		return result;
	}

	std::string BuildLine(LogLevel level_, std::string_view message_)
	{
		return std::format("[{}][{}] {}", GetTimestamp(), ToString(level_), message_);
	}
}

bool Logger::Initialize(const std::filesystem::path& filePath_)
{
	std::scoped_lock lock{ mutex };

	if (!filePath_.empty())
	{
		const std::filesystem::path parentPath{ filePath_.parent_path() };
		if (!parentPath.empty())
		{
			std::error_code errorCode;
			std::filesystem::create_directories(parentPath, errorCode);
		}

		fileStream.open(filePath_, std::ios::out | std::ios::app);
		fileOutputEnabled = fileStream.is_open();
	}

	return !fileOutputEnabled || fileStream.is_open();
}

void Logger::Release()
{
	std::scoped_lock lock{ mutex };

	if (fileStream.is_open())
	{
		fileStream.flush();
		fileStream.close();
	}
}

void Logger::SetMinimumLevel(LogLevel level_) noexcept
{
	std::scoped_lock lock{ mutex };
	minimumLevel = level_;
}

LogLevel Logger::GetMinimumLevel() noexcept
{
	std::scoped_lock lock{ mutex };
	return minimumLevel;
}

void Logger::EnableConsoleOutput(bool enabled_) noexcept
{
	std::scoped_lock lock{ mutex };
	consoleOutputEnabled = enabled_;
}

void Logger::EnableDebugOutput(bool enabled_) noexcept
{
	std::scoped_lock lock{ mutex };
	debugOutputEnabled = enabled_;
}

void Logger::EnableFileOutput(bool enabled_) noexcept
{
	std::scoped_lock lock{ mutex };
	fileOutputEnabled = enabled_;
}

void Logger::Write(LogLevel level_, std::string_view message_)
{
	std::scoped_lock lock{ mutex };

	if (!ShouldWrite(level_))
	{
		return;
	}

	const std::string line{ BuildLine(level_, message_) };

	if (consoleOutputEnabled)
	{
		std::ostream& stream{ level_ >= LogLevel::Warning ? std::cerr : std::cout };
		stream << line << '\n';
	}

	if (debugOutputEnabled)
	{
		::OutputDebugStringA(line.c_str());
		::OutputDebugStringA("\n");
	}

	if (fileOutputEnabled && fileStream.is_open())
	{
		fileStream << line << '\n';
		fileStream.flush();
	}
}

void Logger::Write(LogLevel level_, std::wstring_view message_)
{
	Write(level_, WideToUtf8(message_));
}
