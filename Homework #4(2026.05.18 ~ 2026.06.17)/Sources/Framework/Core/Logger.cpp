#include "Precompiled.hpp"
#include "Logger.hpp"

namespace
{
	using Category = TUK::Framework::Logger::Category;

	auto Temp(Category category)
	{
		switch (category)
		{
		case Category::Trace: return RGB(0, 255, 0);
		case Category::Info: return RGB(255, 255, 255);
		case Category::Warning: return RGB(255, 255, 0);
		case Category::Error: return RGB(255, 0, 0);
		case Category::Critical: return RGB(255, 0, 255);
		default: std::unreachable();
		}
	}

	auto Temp2(Category category)
	{
		switch (category)
		{
		case Category::Trace: return "TRACE";
		case Category::Info: return "INFO";
		case Category::Warning: return "WARNING";
		case Category::Error: return "ERROR";
		case Category::Critical: return "CRITICAL";
		default: std::unreachable();
		}
	}
}

namespace TUK::Framework
{
	void Logger::Log(Category category, const std::string_view& message)
	{
		const std::string formattedMessage = std::format("[{}] {}\n", Temp2(category), message);
		std::printf(formattedMessage.c_str());
	}
}
