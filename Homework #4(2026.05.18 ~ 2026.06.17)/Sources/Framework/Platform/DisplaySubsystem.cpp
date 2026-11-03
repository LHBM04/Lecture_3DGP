#include "Precompiled.hpp"
#include "DisplaySubsystem.hpp"

#include "Resource.h"

namespace
{
	/** 인스턴스 핸들 */
	const HINSTANCE Instance = GetModuleHandleW(nullptr);

	/** 윈도우 클래스 이름 */
	TCHAR wndClassName[256];

	/** 윈도우 클래스 */
	ATOM wndClass = 0;
}

namespace TUK::Framework
{
	DisplaySubsystem::DisplaySubsystem() noexcept
		: EngineSubsystem(2)
	{
	}

	DisplaySubsystem::~DisplaySubsystem() noexcept
	{
		displays.clear();

		if (wndClass != 0)
		{
			UnregisterClassW(wndClassName, Instance);
			wndClass = 0;
		}
	}

	std::expected<std::reference_wrapper<Display>, std::string> DisplaySubsystem::CreateDisplay(const DisplayOptions& options)
	{
		std::unique_ptr<Display> newWindow = std::make_unique<Display>(nullptr);

		HWND hWnd = CreateWindowExW(
			0, 
			wndClassName,
			options.caption,
			WS_OVERLAPPEDWINDOW,
			options.position.GetX(),
			options.position.GetY(),
			options.size.GetX(),
			options.size.GetY(),
			nullptr, 
			nullptr, 
			Instance, 
			newWindow.get());

		if (!hWnd)
		{
			return std::unexpected(std::format("창 생성 실패 (Win32 오류: {}).", GetLastError()));
		}

		auto reference = std::ref(*newWindow);
		displays.push_back(std::move(newWindow));
		ShowWindow(hWnd, SW_SHOW);
		return reference;
	}

	SystemStatus DisplaySubsystem::OnStartup()
	{
		LoadStringW(Instance, IDS_WNDCLASS, wndClassName, sizeof(wndClassName) / sizeof(TCHAR));

		WNDCLASSEXW description{};
		description.cbSize = sizeof(description);
		description.style = CS_HREDRAW | CS_VREDRAW;
		description.lpfnWndProc = WindowProc;
		description.hInstance = Instance;
		description.hCursor = LoadCursorW(Instance, IDC_ARROW);
		description.lpszClassName = wndClassName;
		description.hIcon = LoadIconW(Instance, MAKEINTRESOURCE(IDI_HOMEWORK04));
		
		wndClass = RegisterClassExW(&description);
		if (wndClass == 0)
		{
			return SystemStatus::Failure;
		}

		return SystemStatus::Continue;
	}

	SystemStatus DisplaySubsystem::OnPostTick()
	{
		std::erase_if(displays, [](const auto& display) {
			return display->ShouldClose();
		});

		return !displays.empty() ? SystemStatus::Continue : SystemStatus::Success;
	}

	void DisplaySubsystem::OnShutdown()
	{
		if (wndClass != 0)
		{
			UnregisterClassW(wndClassName, Instance);
			wndClass = 0;
		}
	}
}
