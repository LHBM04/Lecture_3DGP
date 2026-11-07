#include "Precompiled.hpp"
#include "DisplaySubsystem.hpp"

#include "../Core/Engine.hpp"

#include "../Utilities/Logger.hpp"

#include "DisplayEvents.hpp"
#include "EventSubsystem.hpp"
#include "InputEvents.hpp"

namespace TUK::Framework
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		ASSERT(hWnd);
		if (message == WM_NCCREATE)
		{
			const CREATESTRUCTW* creation = reinterpret_cast<const CREATESTRUCTW*>(lParam);
			ASSERT(creation);

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
			case WM_ACTIVATEAPP:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(InputActivationEvent{ { display }, static_cast<bool>(wParam) });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_CREATE:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayCreateEvent{ { display } });
				}
				return 0;
			}
			case WM_SIZE:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayResizeEvent{ { display }, Vector2D<std::int32_t>{ LOWORD(lParam), HIWORD(lParam) } });
				}
				return 0;
			}
			case WM_MOVE:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayMoveEvent{ { display }, Vector2D<std::int32_t>{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) } });
				}
				return 0;
			}
			case WM_SETFOCUS: [[fallthrough]];
			case WM_KILLFOCUS:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayFocusEvent{ { display }, message == WM_SETFOCUS });
				}
				return 0;
			}
			case WM_CLOSE:
			{
				display->SetShouldClose(true);
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayCloseEvent{ { display } });
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			case WM_DESTROY:
			{
				if (eventSubsystem)
				{
					eventSubsystem->PushEvent(DisplayDestroyEvent{ { display } });
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
			LOG_ERROR("윈도우 클래스 등록에 실패했습니다 (Win32 오류: {}).", GetLastError());
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

	std::expected<Display*, std::string> DisplaySubsystem::CreateDisplay(const DisplayOptions& options) noexcept
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
			return std::unexpected(std::format("윈도우 생성에 실패했습니다 (Win32 오류: {}).", GetLastError()));
		}

		Display* pointer = newWindow.get();
		displays.push_back(std::move(newWindow));

		ShowWindow(hWnd, options.show);
		UpdateWindow(hWnd);
		
		return pointer;
	}

	void DisplaySubsystem::DestroyDisplay(Display* display) noexcept
	{
		ASSERT_FMT(display, "삭제할 디스플레이가 nullptr입니다.");
		if (!display)
		{
			return;
		}

		std::erase_if(displays, [display](const std::unique_ptr<Display>& item)
			{
				return item.get() == display;
			});
	}

	usize DisplaySubsystem::GetDisplayCount() const noexcept
	{
		return displays.size();
	}

	Display* DisplaySubsystem::GetDisplay(usize index) noexcept
	{
		return index < displays.size() ? displays[index].get() : nullptr;
	}

	const Display* DisplaySubsystem::GetDisplay(usize index) const noexcept
	{
		return index < displays.size() ? displays[index].get() : nullptr;
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

	Display* DisplaySubsystem::FindDisplayByID(Display::ID id) noexcept
	{
		const auto result = std::ranges::find_if(displays, [id](const auto& display)
			{
				return display->GetID() == id;
			});
		return result != displays.end() ? result->get() : nullptr;
	}

	const Display* DisplaySubsystem::FindDisplayByID(Display::ID id) const noexcept
	{
		const auto result = std::ranges::find_if(displays, [id](const auto& display)
			{
				return display->GetID() == id;
			});
		return result != displays.end() ? result->get() : nullptr;
	}

	bool DisplaySubsystem::IsDisplayValid(const Display* display) const noexcept
	{
		if (!display)
		{
			return false;
		}

		return std::ranges::any_of(displays, [display](const auto& item)
			{
				return item.get() == display;
			});
	}

	bool DisplaySubsystem::IsDisplayValid(HWND hWnd) const noexcept
	{
		return FindDisplayByHWND(hWnd) != nullptr;
	}

	bool DisplaySubsystem::IsDisplayValid(Display::ID id) const noexcept
	{
		return FindDisplayByID(id) != nullptr;
	}
}
