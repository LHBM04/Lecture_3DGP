#pragma once

#include <cstddef>
#include <format>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>

#include "../Core/Common.hpp"

#ifdef ERROR
#undef ERROR
#endif

namespace TUK::Framework
{
	namespace Logger
	{
		enum class Category : u8
		{
			TRACE,
			INFO,
			WARNING,
			ERROR,
			CRITICAL
		};

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

		void Log(Category level, std::string_view message, std::source_location location = std::source_location::current());

		template <class... TArgs>
		void LogTrace(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Log(Category::TRACE, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

		template <class... TArgs>
		void LogInfo(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Log(Category::INFO, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

		template <class... TArgs>
		void LogWarning(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Log(Category::WARNING, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

		template <class... TArgs>
		void LogError(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Log(Category::ERROR, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}

		template <class... TArgs>
		void LogCritical(Format<std::type_identity_t<TArgs>...> format, TArgs&&... args)
		{
			Log(Category::CRITICAL, std::format(format.text, std::forward<TArgs>(args)...), format.location);
		}
	}
}
