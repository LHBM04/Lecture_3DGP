#pragma once

#include <concepts>
#include <format>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>

#include "../Core/Common.hpp"

#ifdef ERROR
	#undef ERROR
#endif

namespace TUK::Framework::Logger
{
	enum class Category
	{
		TRACE,
		INFO,
		WARNING,
		ERROR,
		CRITICAL
	};

	template <class TChar, class... TArgs>
	struct Format final
	{
		template <std::convertible_to<std::basic_string_view<TChar>> T>
		consteval Format(const T& text, std::source_location loc = std::source_location::current()) noexcept
			: text(text)
			, location(loc)
		{
		}

		std::basic_format_string<TChar, TArgs...> text;
		std::source_location location;
	};

	template <class... TArgs>
	using FormatAscii = Format<char, std::type_identity_t<TArgs>...>;

	template <class... TArgs>
	using FormatWide = Format<wchar_t, std::type_identity_t<TArgs>...>;

	void Log(Category level, std::string_view message, const std::source_location& location = std::source_location::current());
	void Log(Category level, std::wstring_view message, const std::source_location& location = std::source_location::current());

	template <class... TArgs>
	void LogTrace(FormatAscii<TArgs...> fmt, TArgs&&... args)
	{
		Log(Category::TRACE, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
	}

	template <class... TArgs>
	void LogTrace(FormatWide<TArgs...> fmt, TArgs&&... args)
	{
		Log(Category::TRACE, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
	}

	template <class... TArgs>
	void LogInfo(FormatAscii<TArgs...> fmt, TArgs&&... args)
	{
		Log(Category::INFO, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
	}

	template <class... TArgs>
	void LogInfo(FormatWide<TArgs...> fmt, TArgs&&... args)
	{
		Log(Category::INFO, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
	}

	template <class... TArgs>
	void LogWarning(FormatAscii<TArgs...> fmt, TArgs&&... args)
	{
		Log(Category::WARNING, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
	}

	template <class... TArgs>
	void LogWarning(FormatWide<TArgs...> fmt, TArgs&&... args)
	{
		Log(Category::WARNING, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
	}

	template <class... TArgs>
	void LogError(FormatAscii<TArgs...> fmt, TArgs&&... args)
	{
		Log(Category::ERROR, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
	}

	template <class... TArgs>
	void LogError(FormatWide<TArgs...> fmt, TArgs&&... args)
	{
		Log(Category::ERROR, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
	}

	template <class... TArgs>
	void LogCritical(FormatAscii<TArgs...> fmt, TArgs&&... args)
	{
		Log(Category::CRITICAL, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
	}

	template <class... TArgs>
	void LogCritical(FormatWide<TArgs...> fmt, TArgs&&... args)
	{
		Log(Category::CRITICAL, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
	}
}

#ifdef _DEBUG
	#define LOG_TRACE(...) TUK::Framework::Logger::LogTrace(__VA_ARGS__)
	#define LOG_INFO(...) TUK::Framework::Logger::LogInfo(__VA_ARGS__)
	#define LOG_WARNING(...) TUK::Framework::Logger::LogWarning(__VA_ARGS__)
	#define LOG_ERROR(...) TUK::Framework::Logger::LogError(__VA_ARGS__)
	#define LOG_CRITICAL(...) TUK::Framework::Logger::LogCritical(__VA_ARGS__)

	#ifndef ASSERT_FMT
		#define ASSERT_FMT(condition, msg, ...) \
				do \
				{ \
					if (!(condition)) \
					{ \
						LOG_CRITICAL(msg __VA_OPT__(,) __VA_ARGS__); \
						ASSERT(condition); \
					} \
				} while (0)
	#endif
#else
	#define LOG_TRACE(...) ((void)0)
	#define LOG_INFO(...) ((void)0)
	#define LOG_WARNING(...) ((void)0)
	#define LOG_ERROR(...) ((void)0)
	#define LOG_CRITICAL(...) ((void)0)

	#define ASSERT_FMT(condition, msg, ...) ((void)0)
#endif
