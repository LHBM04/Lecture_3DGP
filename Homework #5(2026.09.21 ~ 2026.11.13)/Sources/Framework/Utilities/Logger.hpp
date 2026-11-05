#pragma once

#include <cstddef>
#include <format>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>

namespace TUK::Framework
{
	enum class LogLevel
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR_LEVEL
	};

	namespace Logger
	{
		template <typename... TArgs>
		struct Format final
		{
			template <std::size_t SIZE>
			consteval Format(const char (&text)[SIZE], std::source_location location = std::source_location::current()) noexcept
				: text(text)
				, location(location)
			{

			}

			std::format_string<TArgs...> text;
			std::source_location location;
		};

		void Log(LogLevel level, std::string_view message, std::source_location location = std::source_location::current());

		template <typename... TArgs>
		void LogDebug(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Log(LogLevel::DEBUG, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

		template <typename... TArgs>
		void LogInfo(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Log(LogLevel::INFO, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

		template <typename... TArgs>
		void LogWarning(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Log(LogLevel::WARNING, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

		template <typename... TArgs>
		void LogError(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Log(LogLevel::ERROR_LEVEL, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

	}
}
