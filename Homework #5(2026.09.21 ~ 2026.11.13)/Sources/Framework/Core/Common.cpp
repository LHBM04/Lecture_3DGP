#include "Precompiled.hpp"
#include "Common.hpp"

namespace TUK::Framework
{
	namespace
	{
		[[nodiscard]] constexpr std::string_view GetCategoryName(Logger::Category level) noexcept
		{
			switch (level)
			{
				case Logger::Category::TRACE:		return "TRACE";
				case Logger::Category::INFO:		return "INFO";
				case Logger::Category::WARNING:		return "WARNING";
				case Logger::Category::ERROR_LEVEL: return "ERROR";
				case Logger::Category::CRITICAL:	return "CRITICAL";
				default:							break;
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

	namespace Random
	{
		namespace
		{
			std::mt19937 generator{ std::random_device{}() };
		}

		i32 RandInt(i32 min, i32 max)
		{
			ASSERT_FMT(min <= max, "난수 범위의 min은 max 이하여야 합니다.");

			if (min == max)
			{
				return min;
			}

			std::uniform_int_distribution<i32> distribution(min, max);
			return distribution(generator);
		}

		f32 RandFloat(f32 min, f32 max)
		{
			ASSERT_FMT(std::isfinite(min), "난수 범위의 min은 유한한 값이어야 합니다.");
			ASSERT_FMT(std::isfinite(max), "난수 범위의 max는 유한한 값이어야 합니다.");
			ASSERT_FMT(min <= max, "난수 범위의 min은 max 이하여야 합니다.");

			if (min == max)
			{
				return min;
			}

			std::uniform_real_distribution<f64> distribution(min, max);
			return static_cast<f32>(distribution(generator));
		}
	}

	void Logger::Log(Category level, std::string_view message, const std::source_location& location)
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

	void Logger::Log(Category level, std::wstring_view message, const std::source_location& location)
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
