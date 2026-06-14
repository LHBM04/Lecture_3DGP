#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <print>
#include <ranges>
#include <source_location>
#include <system_error>
#include <type_traits>
#include <utility>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <wrl.h>

#include <comdef.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <d3dcommon.h>
#include <d3dcompiler.h>

namespace Debugger
{
	enum class LogCategory : unsigned char
	{
		Trace,
		Info,
		Warning,
		Error,
		Critical
	};

	template <class... TArgs>
	inline void Log(LogCategory category_, std::string_view format_, TArgs&&... args_, std::source_location location_)
	{
		static constexpr std::string_view categoryNames[] = {
			"[Trace]",
			"[Info]",
			"[Warning]",
			"[Error]",
			"[Critical]"
		};

		const std::string_view categoryText =
			categoryNames[static_cast<std::size_t>(category_)];

		std::chrono::local_time<std::chrono::system_clock::duration> nowTime{
			std::chrono::current_zone()->to_local(std::chrono::system_clock::now())
		};

		std::string message{
			std::vformat(format_, std::make_format_args(std::forward<TArgs>(args_)...))
		};

		std::cout << std::format(
			"[{:%H:%M:%OS}] {} [{}:{}] {}\n",
			nowTime,
			categoryText,
			location_.file_name(),
			location_.line(),
			message);
	}

	template <class... TArgs>
	inline void LogInfo(std::string_view format_, TArgs&&... args_, std::source_location location_ = std::source_location::current())
	{
		Log(LogCategory::Trace, format_, std::forward<TArgs>(args_)..., location_);
	}

	template <class... TArgs>
	inline void LogTrace(std::string_view format_, TArgs&&... args_)
	{
		Log(LogCategory::Info, format_, std::forward<TArgs>(args_)..., std::source_location::current());
	}

	template <class... TArgs>
	void LogWarning(std::string_view format_, TArgs&&... args_)
	{
		Log(LogCategory::Warning, format_, std::forward<TArgs>(args_)..., std::source_location::current());
	}

	template <class... TArgs>
	void LogError(std::string_view format_, TArgs&&... args_)
	{
		Log(LogCategory::Error, format_, std::forward<TArgs>(args_)..., std::source_location::current());
	}

	template <class... TArgs>
	void LogCritical(std::string_view format_, TArgs&&... args_)
	{
		Log(LogCategory::Critical, format_, std::forward<TArgs>(args_)..., std::source_location::current());
	}

	inline void Abort()
	{
		do
		{
			__debugbreak();
			::exit(1);
		} while (0);
	}

	inline void Assert(bool condition_, std::string_view message_)
	{
		do
		{
			if (!(condition_))
			{
				LogCritical(message_);
#ifdef _DEBUG
				Abort();
#endif
			}
		} while (0);
	}

	template <class... TArgs>
	inline void Assert(bool condition_, std::string_view message_, TArgs&&... args_)
	{
		do
		{
			if (!(condition_))
			{
				LogCritical(message_, std::forward<TArgs>(args_)...);
#ifdef _DEBUG
				Abort();
#endif
			}
		} while (0);
	}

	inline std::string GetErrorCode(HRESULT hResult_)
	{
		_com_error error{ hResult_ };
		_bstr_t bstr(error.ErrorMessage());

		const char* const message{ bstr };
		return message;
	}

	inline void ThrowIfFailed(HRESULT hResult_)
	{
		if (FAILED(hResult_))
		{
			throw std::runtime_error(GetErrorCode(hResult_));
		}
	}
}
