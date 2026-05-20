#include "Precompiled.hpp"

#include "Renderer.hpp"
#include "Logger.hpp"
#include "Window.hpp"
#include "Timer.hpp"
#include "Input.hpp"

#include "Scene_Game.hpp"

INT APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	if (AllocConsole())
	{
		FILE* pFile{ nullptr };
		freopen_s(&pFile, "CONOUT$", "w", stdout);
		freopen_s(&pFile, "CONOUT$", "w", stderr);

		Logger::Trace("콘솔 창이 생성되었습니다, 로그를 출력합니다...");
		Logger::Info("로그 출력이 정상적으로 이루어지고 있습니다.");
	}
#endif

	// 윈도우 초기화.
	Logger::Trace("윈도우 초기화가 진행 중입니다...");
	{
		Window::Options options
		{
			.hInstance = hInstance,
			.className = L"Homework #1 Class",
			.title = L"Homework #1",
			.x = CW_USEDEFAULT,
			.y = CW_USEDEFAULT,
			.width = 1280,
			.height = 720,
			.isFullscreen = false,
			.isResizable = false,
			.isBorderless = false,
			.isVSync = false
		};

		if (!Window::Initialize(options))
		{
			Logger::Critical("윈도우 초기화에 실패했습니다! 오류 코드: %d", GetLastError());
			return -1;
		}

		Logger::Info("윈도우 초기화가 완료되었습니다.");
	}

	// 렌더러 초기화.
	Logger::Trace("렌더러 초기화가 진행 중입니다...");
	{
		Renderer::Options options
		{
			.hWnd = Window::GetHWND(),
			.width = 1280,
			.height = 720,
			.clearColor = ColorRGBA::GetBlack()
		};

		if (!Renderer::Initialize(options))
		{
			Logger::Critical("렌더러 초기화에 실패했습니다! 오류 코드: %d", GetLastError());
			return -1;
		}
		Renderer::SetPolygonMode(Renderer::PolygonMode::Solid);

		Logger::Info("렌더러 초기화가 완료되었습니다.");
	}

	Timer::Initialize();

	Scene_Game scene;
	scene.Load();

	while (!Window::ShouldClose())
	{
		Input::Update();
		Timer::Update();

		Window::PollEvent();

#if defined(_DEBUG)
		// 근데 프레임이 제대로 계산이 되고 있는 건가
		WCHAR title[256];
		swprintf_s(title, L"Homework #1 - FPS: %.1f", Timer::GetFPS());

		Window::SetTitle(title);

		if (Input::IsKeyPressed(VK_RETURN))
		{
			if (Renderer::GetPolygonMode() == Renderer::PolygonMode::Solid)
			{
				Renderer::SetPolygonMode(Renderer::PolygonMode::Wire);
				Logger::Info("폴리곤 모드가 와이어프레임으로 변경되었습니다.");
			}
			else
			{
				Renderer::SetPolygonMode(Renderer::PolygonMode::Solid);
				Logger::Info("폴리곤 모드가 솔리드로 변경되었습니다.");
			}
		}
#endif

		scene.Update();
		Renderer::Clear();
		scene.Render();
		Renderer::Present();
	}

	scene.Unload();

	Renderer::Terminate();
	Window::Terminate();

	return 0;
}
