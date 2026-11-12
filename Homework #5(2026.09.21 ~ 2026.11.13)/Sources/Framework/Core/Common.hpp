#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <functional>
#include <memory>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace TUK::Framework
{
	using i8 = std::int8_t;
	using i16 = std::int16_t;
	using i32 = std::int32_t;
	using i64 = std::int64_t;

	using u8 = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;

	using f32 = float;
	using f64 = double;

	using usize = std::size_t;
	using isize = std::ptrdiff_t;

	using byte = std::byte;

	template <class TValue>
	concept Scalarable = std::same_as<TValue, i32> || std::same_as<TValue, f32>;

	/** C# Func */
	template <class TResult, class... TArgs>
	using Func = std::function<TResult(TArgs...)>;

	/** C# Action */
	template <class... TArgs>
	using Action = std::function<void(TArgs...)>;

	/** C# Predicate */
	template <class... TArgs>
	using Predicate = std::function<bool(TArgs...)>;

	/** Delegate */

	template <class... TArgs>
	class Delegate
	{
	public:
		using Listener = Action<TArgs...>;
		using ListenerId = usize;

		Delegate() noexcept = default;
		virtual ~Delegate() noexcept = default;

		Delegate(const Delegate&) noexcept = delete;
		Delegate& operator=(const Delegate&) noexcept = delete;

		Delegate(Delegate&&) noexcept = default;
		Delegate& operator=(Delegate&&) noexcept = default;

		Delegate& operator+=(Listener callback) noexcept;

		ListenerId AddListener(Listener callback);
		void RemoveListener(ListenerId id) noexcept;
		
		void Broadcast(TArgs... args) const;

		void Clear() noexcept;

	private:
		struct Subscription final
		{
			ListenerId id;
			Listener callback;
			bool isConnected = true;
		};

		std::vector<std::shared_ptr<Subscription>> subscriptions;
		ListenerId nextListenerId = 1;
	};

	template <class... TArgs>
	Delegate<TArgs...>& Delegate<TArgs...>::operator+=(Listener callback) noexcept
	{
		AddListener(std::move(callback));
		return *this;
	}

	template <class... TArgs>
	typename Delegate<TArgs...>::ListenerId Delegate<TArgs...>::AddListener(Listener callback)
	{
		const ListenerId id = nextListenerId++;
		subscriptions.push_back(std::make_shared<Subscription>(Subscription{ id, std::move(callback) }));
		return id;
	}

	template <class... TArgs>
	void Delegate<TArgs...>::RemoveListener(ListenerId id) noexcept
	{
		const auto subscription = std::ranges::find(subscriptions, id, &Subscription::id);
		if (subscription != subscriptions.end())
		{
			(*subscription)->isConnected = false;
			subscriptions.erase(subscription);
		}
	}

	template <class... TArgs>
	void Delegate<TArgs...>::Clear() noexcept
	{
		for (const auto& subscription : subscriptions)
		{
			subscription->isConnected = false;
		}
		subscriptions.clear();
	}

	template <class... TArgs>
	void Delegate<TArgs...>::Broadcast(TArgs... args) const
	{
		const auto snapshot = subscriptions;
		for (const auto& subscription : snapshot)
		{
			if (subscription->isConnected && subscription->callback)
			{
				subscription->callback(args...);
			}
		}
	}

	namespace Logger
	{
		enum class Category
		{
			TRACE,
			INFO,
			WARNING,
			ERROR_LEVEL,
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
			Log(Category::ERROR_LEVEL, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
		}

		template <class... TArgs>
		void LogError(FormatWide<TArgs...> fmt, TArgs&&... args)
		{
			Log(Category::ERROR_LEVEL, std::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
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
}

#ifdef _DEBUG
	#define LOG_TRACE(...) TUK::Framework::Logger::LogTrace(__VA_ARGS__)
	#define LOG_INFO(...) TUK::Framework::Logger::LogInfo(__VA_ARGS__)
	#define LOG_WARNING(...) TUK::Framework::Logger::LogWarning(__VA_ARGS__)
	#define LOG_ERROR(...) TUK::Framework::Logger::LogError(__VA_ARGS__)
	#define LOG_CRITICAL(...) TUK::Framework::Logger::LogCritical(__VA_ARGS__)
#else
	#define LOG_TRACE(...) ((void)0)
	#define LOG_INFO(...) ((void)0)
	#define LOG_WARNING(...) ((void)0)
	#define LOG_ERROR(...) ((void)0)
	#define LOG_CRITICAL(...) ((void)0)
#endif

#ifndef ABORT
	#define ABORT() \
			do \
			{ \
				__debugbreak(); \
				::exit(1); \
			} while (0)
#endif

#ifndef ASSERT
	#define ASSERT(condition) \
				do \
				{ \
					if (!(condition)) \
					{ \
						ABORT(); \
					} \
				} while (0)
#endif

#ifndef ASSERT_FMT
	#define ASSERT_FMT(condition, msg, ...) \
			do \
			{ \
				if (!(condition)) \
				{ \
					LOG_CRITICAL(msg __VA_OPT__(,) __VA_ARGS__); \
					ABORT(); \
				} \
			} while (0)
#endif
