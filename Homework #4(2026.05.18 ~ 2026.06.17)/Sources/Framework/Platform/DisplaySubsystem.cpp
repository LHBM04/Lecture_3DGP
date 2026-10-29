#include "Precompiled.hpp"
#include "DisplaySubsystem.hpp"

namespace
{
	/** 윈도우 클래스 이름 */
	constexpr LPCSTR ClassName = "TUK.Framework.Window";

	/** 인스턴스 핸들 */
	const HINSTANCE Instance = GetModuleHandleA(nullptr);

	/** 윈도우 클래스 */
	bool isWndClassRegistered = false;
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

		if (isWndClassRegistered)
		{
			UnregisterClassA(ClassName, Instance);
			isWndClassRegistered = false;
		}
	}

	std::expected<std::reference_wrapper<Display>, std::string> DisplaySubsystem::CreateDisplay(const DisplayOptions& options)
	{
		std::unique_ptr<Display> newWindow = std::make_unique<Display>(nullptr);

		HWND hWnd = CreateWindowExA(
			0, 
			ClassName, 
			options.caption.data(),
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
		WNDCLASSEXA description{};
		description.cbSize = sizeof(description);
		description.style = CS_HREDRAW | CS_VREDRAW;
		description.lpfnWndProc = WindowProc;
		description.hInstance = Instance;
		description.hCursor = LoadCursorA(nullptr, IDC_ARROW);
		description.lpszClassName = ClassName;
		description.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
		
		if (RegisterClassExA(&description) == 0)
		{
			return SystemStatus::Failure;
		}

		isWndClassRegistered = true;

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
		if (isWndClassRegistered)
		{
			UnregisterClassA(ClassName, Instance);
			isWndClassRegistered = false;
		}
	}
}
