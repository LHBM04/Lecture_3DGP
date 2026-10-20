#pragma once

namespace TUK::Framework
{
	namespace Logger
	{
		enum class Category : std::uint8_t
		{
			Trace,
			Info,
			Warning,
			Error,
			Critical,
		};

		template <class... TArgs>
		void Trace(const std::string_view& format, TArgs&&... args)
		{
			const auto message = std::vformat(format, std::make_format_args(std::forward<TArgs>(args)...));
			Log(Category::Trace, message);
		}

		template <class... TArgs>
		void Info(const std::string_view& format, TArgs&&... args)
		{
			const auto message = std::vformat(format, std::make_format_args(std::forward<TArgs>(args)...));
			Log(Category::Info, message);
		}

		template <class... TArgs>
		void Warning(const std::string_view& format, TArgs&&... args)
		{
			const auto message = std::vformat(format, std::make_format_args(std::forward<TArgs>(args)...));
			Log(Category::Warning, message);
		}

		template <class... TArgs>
		void Error(const std::string_view& format, TArgs&&... args)
		{
			const auto message = std::vformat(format, std::make_format_args(std::forward<TArgs>(args)...));
			Log(Category::Error, message);
		}

		template <class... TArgs>
		void Critical(const std::string_view& format, TArgs&&... args)
		{
			const auto message = std::vformat(format, std::make_format_args(std::forward<TArgs>(args)...));
			Log(Category::Critical, message);
		}

		void Log(Category category, const std::string_view& message);
	}
}
