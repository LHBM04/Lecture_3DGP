#pragma once

#include <string>
#include <string_view>

namespace TUK::Framework
{
	class Window
	{
	public:
		Window() = default;
		virtual ~Window() = default;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		[[nodiscard]] virtual std::wstring GetTitle() const = 0;
		virtual void SetTitle(std::wstring_view title) = 0;

		[[nodiscard]] virtual int GetSizeX() const = 0;
		virtual void SetSizeX(int sizeX) = 0;

		[[nodiscard]] virtual int GetSizeY() const = 0;
		virtual void SetSizeY(int sizeY) = 0;

		[[nodiscard]] virtual int GetPositionX() const = 0;
		virtual void SetPositionX(int positionX) = 0;

		[[nodiscard]] virtual int GetPositionY() const = 0;
		virtual void SetPositionY(int positionY) = 0;
	};
}
