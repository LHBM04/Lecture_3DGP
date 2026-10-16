#pragma once

#include <string>
#include <string_view>

#include "WindowOptions.hpp"

namespace TUK::Framework
{
	class Window
	{
		friend class EventSubsystem;
		friend class WindowSubsystem;

	public:
		virtual ~Window() noexcept = default;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&&) = default;
		Window& operator=(Window&&) = default

#if defined(_UNICODE) || defined(UNICODE)
		[[nodiscard]] virtual const std::wstring& GetCaption() const = 0;
		virtual void SetCaption(std::wstring_view value) noexcept = 0;
#else
		[[nodiscard]] virtual const std::string& GetCaption() const = 0;
		virtual void SetCaption(std::string_view value) noexcept = 0;
#endif

		[[nodiscard]] virtual Vector2D<int> GetPosition() const noexcept = 0;
		virtual void SetPosition(const Vector2D<int>& position) noexcept = 0;

		[[nodiscard]] virtual Vector2D<int> GetSize() const noexcept = 0;
		virtual void SetSize(const Vector2D<int>& size) noexcept = 0;

	protected:
		Window() noexcept = default;
	};
}

