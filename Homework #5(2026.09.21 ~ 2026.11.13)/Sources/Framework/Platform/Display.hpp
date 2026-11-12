#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "../Core/Common.hpp"

#include "../Math/Vector2D.hpp"

namespace TUK::Framework
{
	class Display
	{
		friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:
		enum class ID : u32{};

		explicit Display(HWND hWnd) noexcept;
		~Display() noexcept;

		Display(const Display&) = delete;
		Display& operator=(const Display&) = delete;

		Display(Display&&) = default;
		Display& operator=(Display&&) = default;

		[[nodiscard]] HWND GetHWND() const noexcept;
		[[nodiscard]] ID GetID() const noexcept;

		[[nodiscard]] LPCTSTR GetCaption() const noexcept;
		void SetCaption(LPCTSTR value) noexcept;

		[[nodiscard]] Vector2D<i32> GetPosition() const noexcept;
		void SetPosition(const Vector2D<i32>& position) noexcept;

		[[nodiscard]] Vector2D<i32> GetSize() const noexcept;
		void SetSize(const Vector2D<i32>& size) noexcept;

		[[nodiscard]] Vector2D<i32> GetClientSize() const noexcept;
		void SetClientSize(const Vector2D<i32>& size) noexcept;

		[[nodiscard]] bool ShouldClose() const noexcept;
		void SetShouldClose(bool value) noexcept;

		[[nodiscard]] DWORD GetStyle() const noexcept;
		void SetStyle(DWORD value) noexcept;

		[[nodiscard]] DWORD GetStyleEx() const noexcept;
		void SetStyleEx(DWORD value) noexcept;

	private:
		inline static ID nextId = static_cast<ID>(0);

		HWND hWnd;
		bool shouldClose;
		ID id;
	};
}
