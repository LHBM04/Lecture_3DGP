#include "Precompiled.hpp"
#include "Common.hpp"

#include <iostream>
#include <string>
#include <syncstream>

namespace TUK::Framework::Logger
{
	namespace
	{
		[[nodiscard]] constexpr std::string_view GetCategoryName(Category level) noexcept
		{
			switch (level)
			{
				case Category::TRACE:    return "TRACE";
				case Category::INFO:     return "INFO";
				case Category::WARNING:  return "WARNING";
				case Category::ERROR_LEVEL:    return "ERROR";
				case Category::CRITICAL: return "CRITICAL";
				default:				 break;
			}

			std::unreachable();
		}

		[[nodiscard]] std::wstring AsciiToWide(std::string_view utf8)
		{
			if (utf8.empty())
			{
				return {};
			}

			const int size = static_cast<int>(utf8.size());
			const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), size, nullptr, 0);
			if (length <= 0)
			{
				return {};
			}

			std::wstring wide(static_cast<std::size_t>(length), L'\0');
			MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), size, wide.data(), length);
			return wide;
		}

		[[nodiscard]] std::string WideToAscii(std::wstring_view wide)
		{
			if (wide.empty())
			{
				return {};
			}

			const int size = static_cast<int>(wide.size());
			const int length = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide.data(), size, nullptr, 0, nullptr, nullptr);
			if (length <= 0)
			{
				return {};
			}

			std::string utf8(static_cast<std::size_t>(length), '\0');
			WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide.data(), size, utf8.data(), length, nullptr, nullptr);
			return utf8;
		}
	}

	void Log(Category level, std::string_view message, const std::source_location& location)
	{
		const std::string line = std::format(
			"[{}] {} ({}:{})\n",
			GetCategoryName(level),
			message,
			location.file_name(),
			location.line()
		);

		std::osyncstream(std::clog) << line << std::flush;

		const std::wstring wideLine = AsciiToWide(line);
		if (!wideLine.empty())
		{
			OutputDebugStringW(wideLine.c_str());
		}
	}

	void Log(Category level, std::wstring_view message, const std::source_location& location)
	{
		const std::string utf8Message = WideToAscii(message);
		const std::string line = std::format(
			"[{}] {} ({}:{})\n",
			GetCategoryName(level),
			utf8Message,
			location.file_name(),
			location.line()
		);

		std::osyncstream(std::clog) << line << std::flush;

		const std::wstring wideLine = AsciiToWide(line);
		if (!wideLine.empty())
		{
			OutputDebugStringW(wideLine.c_str());
		}
	}
}