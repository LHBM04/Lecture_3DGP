#pragma once

#include <string>
#include <string_view>

#include "DisplayOptions.hpp"

namespace TUK::Framework
{
	class Display
	{
		friend class EventSubsystem;
		friend class DisplaySubsystem;

	public:
		virtual ~Display() noexcept = default;

		Display(const Display&) = delete;
		Display& operator=(const Display&) = delete;

		Display(Display&&) = default;
		Display& operator=(Display&&) = default;

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
		Display() noexcept = default;
	};
}

