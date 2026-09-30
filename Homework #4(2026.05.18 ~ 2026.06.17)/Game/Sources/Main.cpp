#include "Precompiled.h"

Framework::Engine* engine = nullptr;

static void Awake()
{
	engine = new Framework::Engine();

	// Window Subsystem 추가
	{
		auto result = engine->AddSubsystem<Framework::WindowSubsystem>();
		if (!result.has_value())
		{
			std::exit(EXIT_FAILURE);
		}
	}
	// Render Subsystem 추가
	{
		auto result = engine->AddSubsystem<Framework::RenderSubsystem>();
		if (!result.has_value())
		{
			std::exit(EXIT_FAILURE);
		}
	}

	engine->Startup();
}

static void Start()
{
	auto result = engine->GetSubsystem<Framework::WindowSubsystem>();
	if (!result.has_value())
	{
		std::exit(EXIT_FAILURE);
	}

	// 창 생성
	{
		auto& windowSubsystem = result.value().get();

		Framework::WindowOptions options{};
		options.title = L"New Window";
		options.positionX = 100;
		options.positionY = 100;
		options.sizeX = 1280;
		options.sizeY = 960;
		options.flags = Framework::WindowFlags::Resizable;

		auto result = windowSubsystem.AddWindow(options);
		if (!result.has_value())
		{
			std::exit(EXIT_FAILURE);
		}

		auto& window = result.value().get();

		std::println("새 창을 생성했습니다.");
		std::println("창의 타이틀: {}", "new title");
		std::println("창의 위치: x={}, y={}", window.GetPositionX(), window.GetPositionY());
		std::println("창의 크기: width={}, height={}", window.GetSizeX(), window.GetSizeY());
	}
}

int main(int, char*[])
{
	Awake();
	Start();

	engine->Run();
	engine->Shutdown();
}
