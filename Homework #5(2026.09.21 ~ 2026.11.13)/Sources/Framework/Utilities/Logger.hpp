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
			consteval Format(const char (&text)[SIZE], std::source_location location = std::source_location::current()) noexcept;

			std::format_string<TArgs...> text;
			std::source_location location;
		};

		template <typename... TArgs>
		template <std::size_t SIZE>
		consteval Format<TArgs...>::Format(const char (&text)[SIZE], std::source_location location) noexcept
			: text(text)
			, location(location)
		{
		}

		void Write(LogLevel level, std::string_view message, std::source_location location = std::source_location::current());

		template <typename... TArgs>
		void Debug(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args);

		template <typename... TArgs>
		void Info(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args);

		template <typename... TArgs>
		void Warning(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args);

		template <typename... TArgs>
		void Error(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args);

		template <typename... TArgs>
		void Debug(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Write(LogLevel::DEBUG, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

		template <typename... TArgs>
		void Info(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Write(LogLevel::INFO, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

		template <typename... TArgs>
		void Warning(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Write(LogLevel::WARNING, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

		template <typename... TArgs>
		void Error(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Write(LogLevel::ERROR_LEVEL, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}
	}
}
