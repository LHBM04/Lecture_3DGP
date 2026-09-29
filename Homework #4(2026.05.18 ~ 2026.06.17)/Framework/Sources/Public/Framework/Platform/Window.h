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

		[[nodiscard]] virtual std::wstring GetTitle() const noexcept = 0;
		virtual void SetTitle(std::wstring_view title) noexcept = 0;

		[[nodiscard]] virtual int GetSizeX() const noexcept = 0;
		virtual void SetSizeX(int sizeX) noexcept = 0;

		[[nodiscard]] virtual int GetSizeY() const noexcept = 0;
		virtual void SetSizeY(int sizeY) noexcept = 0;

		[[nodiscard]] virtual int GetPositionX() const noexcept = 0;
		virtual void SetPositionX(int positionX) noexcept = 0;

		[[nodiscard]] virtual int GetPositionY() const noexcept = 0;
		virtual void SetPositionY(int positionY) noexcept = 0;
	};

	Window* AddWindow();
}
