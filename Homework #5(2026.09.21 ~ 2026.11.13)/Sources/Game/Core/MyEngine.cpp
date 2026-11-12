#include "Precompiled.hpp"
#include "MyEngine.hpp"

#include "Framework/Platform.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Core/Common.hpp"

#include "Time.hpp"

namespace TUK::Game
{
	SystemStatus MyEngine::Startup()
	{
		OnSubsystemAdded<TimeSubsystem>(&Time::Initialize);
		OnSubsystemRemoved<TimeSubsystem>([](TimeSubsystem*)
		{
			Time::Release();
		});
		OnSubsystemAdded<RenderSubsystem>(this, &MyEngine::OnRenderSubsystemAdded);

		AddSubsystem<TimeSubsystem>();

		displaySubsystem = AddSubsystem<DisplaySubsystem>();
		if (!displaySubsystem)
		{
			LOG_ERROR("디스플레이 서브시스템을 추가하지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		eventSubsystem = AddSubsystem<EventSubsystem>();
		if (!eventSubsystem)
		{
			LOG_ERROR("이벤트 서브시스템을 추가하지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		renderSubsystem = AddSubsystem<RenderSubsystem>();
		if (!renderSubsystem)
		{
			LOG_ERROR("렌더링 서브시스템을 추가하지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		return Engine::Startup();
	}

	void MyEngine::OnRenderSubsystemAdded(RenderSubsystem* subsystem)
	{
		ASSERT_FMT(eventSubsystem, "이벤트 서브시스템이 nullptr입니다.");

		eventSubsystem->Subscribe<DisplayCreateEvent>(subsystem, &RenderSubsystem::OnDisplayCreate);
		eventSubsystem->Subscribe<DisplayCloseEvent>(subsystem, &RenderSubsystem::OnDisplayClose);
	}
}
