#include "Precompiled.hpp"
#include "Logger.hpp"

namespace TUK::Framework
{
	namespace
	{
		[[nodiscard]] std::string_view GetCategoryName(Logger::Category level) noexcept
		{
			switch (level)
			{
			case Logger::Category::TRACE:
				return "TRACE";
			case Logger::Category::INFO:
				return "INFO";
			case Logger::Category::WARNING:
				return "WARNING";
			case Logger::Category::ERROR:
				return "ERROR";
			case Logger::Category::CRITICAL:
				return "CRITICAL";
			default:
				return "UNKNOWN";
			}
		}
	}

	namespace Logger
	{
		void Log(Category level, std::string_view message, std::source_location location)
		{
			LPCTSTR line = std::format(TEXT("[{}] {} ({}:{})\n"), GetCategoryName(level), message, location.file_name(), location.line()).c_str();
			std::osyncstream(std::clog) << line << std::flush;
			OutputDebugString(line);
		}
	}
}
