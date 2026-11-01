#include "Precompiled.hpp"
#include "DisplaySubsystem.hpp"

#include "Resource.h"

#include "../Core/Engine.hpp"

#include "DisplayEvents.hpp"
#include "EventSubsystem.hpp"
#include "InputEvents.hpp"

namespace TUK::Framework
{
	namespace
	{
		[[nodiscard]] KeyCode ResolveKey(WPARAM wParam, LPARAM lParam) noexcept
		{
			if (wParam == VK_SHIFT)
			{
				const UINT SCANCODE = (static_cast<UINT>(lParam) >> 16) & 0xFFu;
				return static_cast<KeyCode>(MapVirtualKey(SCANCODE, MAPVK_VSC_TO_VK_EX));
			}

			if (wParam == VK_CONTROL)
			{
				return (lParam & 0x01000000) != 0 ? KeyCode::RIGHT_CONTROL : KeyCode::LEFT_CONTROL;
			}

			if (wParam == VK_MENU)
			{
				return (lParam & 0x01000000) != 0 ? KeyCode::RIGHT_ALT : KeyCode::LEFT_ALT;
			}

			return static_cast<KeyCode>(wParam);
		}

		[[nodiscard]] ButtonCode ResolveButton(UINT message, WPARAM wParam) noexcept
		{
			switch (message)
			{
				case WM_LBUTTONDOWN: [[fallthrough]];
				case WM_LBUTTONUP:
				{
					return ButtonCode::LEFT;
				}
				case WM_RBUTTONDOWN: [[fallthrough]];
				case WM_RBUTTONUP:
				{
					return ButtonCode::RIGHT;
				}
				case WM_MBUTTONDOWN: [[fallthrough]];
				case WM_MBUTTONUP:
				{
					return ButtonCode::MIDDLE;
				}
				case WM_XBUTTONDOWN: [[fallthrough]];
				case WM_XBUTTONUP:
				{
					return GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? ButtonCode::X1 : ButtonCode::X2;
				}
				default:
				{
					return ButtonCode::NONE;
				}
			}
		}
	}

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_NCCREATE)
		{
			const CREATESTRUCTW* creation = reinterpret_cast<const CREATESTRUCTW*>(lParam);

			Display* display = static_cast<Display*>(creation->lpCreateParams);
			if (!display)
			{
				return FALSE;
			}

			display->hWnd = hWnd;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(display));

			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		Display* display = reinterpret_cast<Display*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (!display)
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		EventSubsystem* eventSubsystem = Engine::GetInstance().GetSubsystem<EventSubsystem>();

		switch (message)
		{
			case WM_KEYDOWN: [[fallthrough]];
			case WM_SYSKEYDOWN:
			{
				if (eventSubsystem && (lParam & (1LL << 30)) == 0)
				{
					eventSubsystem->PushEvent(KeyInterectEvent{ { *display }, ResolveKey(wParam, lParam), InputAction::PRESSED });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_KEYUP: [[fallthrough]];
			case WM_SYSKEYUP:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(KeyInterectEvent{ { *display }, ResolveKey(wParam, lParam), InputAction::RELEASED });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_LBUTTONDOWN: [[fallthrough]];
			case WM_MBUTTONDOWN: [[fallthrough]];
			case WM_RBUTTONDOWN: [[fallthrough]];
			case WM_XBUTTONDOWN:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(ButtonInterectEvent{ { *display }, ResolveButton(message, wParam), InputAction::PRESSED });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_LBUTTONUP: [[fallthrough]];
			case WM_MBUTTONUP: [[fallthrough]];
			case WM_RBUTTONUP: [[fallthrough]];
			case WM_XBUTTONUP:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(ButtonInterectEvent{ { *display }, ResolveButton(message, wParam), InputAction::RELEASED });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_MOUSEMOVE:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(MouseMoveEvent{ { *display }, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) } });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_MOUSEWHEEL:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(MouseWheelEvent{ { *display }, { 0, GET_WHEEL_DELTA_WPARAM(wParam) } });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_MOUSEHWHEEL:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(MouseWheelEvent{ { *display }, { GET_WHEEL_DELTA_WPARAM(wParam), 0 } });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_MOUSEHOVER:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(MouseHoverEvent{ { *display } });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_ACTIVATEAPP:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(InputActivationEvent{ { *display }, static_cast<bool>(wParam) });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_INPUT:
			{
				if (eventSubsystem)
				{
					RAWINPUT input{};
					UINT size = sizeof(input);
					if (GetRawInputData(
						reinterpret_cast<HRAWINPUT>(lParam),
						RID_INPUT,
						&input,
						&size,
						sizeof(RAWINPUTHEADER)) != static_cast<UINT>(-1)
						&& input.header.dwType == RIM_TYPEMOUSE)
					{
						const RAWMOUSE& mouse = input.data.mouse;
						if ((mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == 0)
						{
							eventSubsystem->DispatchEvent(MouseRawInputEvent{ { *display }, { static_cast<i32>(mouse.lLastX), static_cast<i32>(mouse.lLastY) } });
						}
					}
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_CREATE:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayCreateEvent{ { *display } });
				}
				return 0;
			}
			case WM_SIZE:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayResizeEvent{ { *display }, Vector2D<std::int32_t>{ LOWORD(lParam), HIWORD(lParam) } });
				}
				return 0;
			}
			case WM_MOVE:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayMoveEvent{ { *display }, Vector2D<std::int32_t>{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) } });
				}
				return 0;
			}
			case WM_SETFOCUS: [[fallthrough]];
			case WM_KILLFOCUS:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayFocusEvent{ { *display }, message == WM_SETFOCUS });
				}
				return 0;
			}
			case WM_CLOSE:
			{
				display->SetShouldClose(true);
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayCloseEvent{ { *display } });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_DESTROY:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayDestroyEvent{ { *display } });
				}
				return 0;
			}
			case WM_NCDESTROY:
			{
				SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
				display->hWnd = nullptr;
				return 0;
			}
			default:
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}

		std::unreachable();
	}

	SystemStatus DisplaySubsystem::OnStartup()
	{
		LoadString(instance, IDS_WNDCLASS, wndClassName, sizeof(wndClassName) / sizeof(TCHAR));

		WNDCLASSEX windowClass{};
		windowClass.cbSize = sizeof(windowClass);
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = instance;
		windowClass.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_HOMEWORK05));
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.lpszClassName = wndClassName;
		windowClass.hIconSm = windowClass.hIcon;

		if (wndClass = RegisterClassEx(&windowClass); wndClass == 0)
		{
			return SystemStatus::FAILURE;
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus DisplaySubsystem::OnPreTick()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus DisplaySubsystem::OnTick()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus DisplaySubsystem::OnPostTick()
	{
		const usize removed = std::erase_if(displays, [](const std::unique_ptr<Display>& display)
			{
				return display->ShouldClose();
			});
		if (removed != 0 && displays.empty())
		{
			return SystemStatus::SUCCESS;
		}
		return SystemStatus::CONTINUE;
	}

	void DisplaySubsystem::OnShutdown()
	{
		displays.clear();
		if (wndClass)
		{
			UnregisterClass(wndClassName, instance);
			wndClass = 0;
		}
		instance = nullptr;
	}

	std::expected<std::reference_wrapper<Display>, std::string> DisplaySubsystem::CreateDisplay(const DisplayOptions& options) noexcept
	{
		std::unique_ptr<Display> newWindow = std::make_unique<Display>(nullptr);

		HWND hWnd = CreateWindowEx(
			options.styleEx,
			wndClassName,
			options.caption,
			options.style,
			options.position.GetX(),
			options.position.GetY(),
			options.size.GetX(),
			options.size.GetY(),
			nullptr,
			nullptr,
			instance,
			newWindow.get());

		if (!hWnd)
		{
			return std::unexpected(std::format("창 생성 실패 (Win32 오류: {}).", GetLastError()));
		}

		auto reference = std::ref(*newWindow);
		displays.push_back(std::move(newWindow));

		ShowWindow(hWnd, options.show);
		UpdateWindow(hWnd);
		
		return reference;
	}

	void DisplaySubsystem::DestroyDisplay(Display& display) noexcept
	{
		std::erase_if(displays, [&display](const std::unique_ptr<Display>& item)
			{
				return item.get() == &display;
			});
	}

	usize DisplaySubsystem::GetDisplayCount() const noexcept
	{
		return displays.size();
	}

	Display& DisplaySubsystem::GetDisplay(usize index) noexcept
	{
		return *displays[index];
	}

	const Display& DisplaySubsystem::GetDisplay(usize index) const noexcept
	{
		return *displays[index];
	}

	Display* DisplaySubsystem::FindDisplayByHWND(HWND hWnd) noexcept
	{
		const auto result = std::ranges::find_if(displays, [hWnd](const auto& display)
			{
				return display->GetHWND() == hWnd;
			});
		return result != displays.end() ? result->get() : nullptr;
	}

	const Display* DisplaySubsystem::FindDisplayByHWND(HWND hWnd) const noexcept
	{
		const auto result = std::ranges::find_if(displays, [hWnd](const auto& display)
			{
				return display->GetHWND() == hWnd;
			});
		return result != displays.end() ? result->get() : nullptr;
	}

	Display* DisplaySubsystem::FindDisplayByID(u32 id) noexcept
	{
		const auto result = std::ranges::find_if(displays, [id](const auto& display)
			{
				return display->GetID() == id;
			});
		return result != displays.end() ? result->get() : nullptr;
	}

	const Display* DisplaySubsystem::FindDisplayByID(u32 id) const noexcept
	{
		const auto result = std::ranges::find_if(displays, [id](const auto& display)
			{
				return display->GetID() == id;
			});
		return result != displays.end() ? result->get() : nullptr;
	}

	bool DisplaySubsystem::IsDisplayValid(const Display& display) const noexcept
	{
		return std::ranges::any_of(displays, [&display](const auto& item)
			{
				return item.get() == &display;
			});
	}

	bool DisplaySubsystem::IsDisplayValid(HWND hWnd) const noexcept
	{
		return FindDisplayByHWND(hWnd) != nullptr;
	}

	bool DisplaySubsystem::IsDisplayValid(u32 id) const noexcept
	{
		return FindDisplayByID(id) != nullptr;
	}
}
