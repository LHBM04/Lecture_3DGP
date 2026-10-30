#include "Precompiled.hpp"
#include "MyApp.hpp"

#include <cassert>
#include <memory>

#include "Framework/Resources.hpp"

#include "Framework/Entities/WorldSubsystem.hpp"

#include "Framework/Graphics/RenderSubsystem.hpp"

#include "Framework/Platform/DisplayCreateInfo.hpp"
#include "Framework/Platform/DisplaySubsystem.hpp"

#include "MyEngine.hpp"
#include "TestWorld.hpp"

DEFINE_APP_INSTANCE(::TUK::Game::MyApp)

namespace TUK::Game
{
	MyApp::MyApp() noexcept
		: window(nullptr)
	{
	}

	MyApp::~MyApp() noexcept
	{
		if (window)
		{
			window->SetShouldClose(true);
			window = nullptr;
		}
	}

	SystemStatus MyApp::Startup()
	{
		Framework::DisplayCreateInfo options{};
		options.caption = "Homework #4(2026.05.18 ~ 2026.06.17)";
		options.position = Framework::Vector2D(100, 100);
		options.size = Framework::Vector2D(1280, 960);

		Framework::DisplaySubsystem* windowSubsystem = MyEngine::GetInstance()->GetSubsystem<Framework::DisplaySubsystem>();
		assert(windowSubsystem);

		auto created = windowSubsystem->CreateDisplay(options);
		if (!created)
		{
			return SystemStatus::Failure;
		}
		window = &(created.value().get());

		Framework::RenderSubsystem* renderSubsystem = MyEngine::GetInstance()->GetSubsystem<Framework::RenderSubsystem>();
		assert(renderSubsystem);
		const Framework::GPUDevice* device = renderSubsystem->GetGPUDevice();
		if (!device)
		{
			return SystemStatus::Failure;
		}

		Framework::WorldSubsystem* worldSubsystem = AddSubsystem<Framework::WorldSubsystem>();
		worldSubsystem->AddWorld(std::make_unique<TestWorld>());
		renderSubsystem->Subscribe(worldSubsystem, &Framework::WorldSubsystem::PreRender);
		renderSubsystem->Subscribe(worldSubsystem, &Framework::WorldSubsystem::Render);
		renderSubsystem->Subscribe(worldSubsystem, &Framework::WorldSubsystem::PostRender);

		Framework::AssetSubsystem* assetSubsystem = AddSubsystem<Framework::AssetSubsystem>();
		assetSubsystem->AddLoader<Framework::ShaderAssetLoader>(*renderSubsystem->GetGPUDevice());
		assetSubsystem->AddLoader<Framework::ModelAssetLoader>(*device, *assetSubsystem);
		assetSubsystem->AddLoader<Framework::MaterialAssetLoader>(*device, *assetSubsystem);

		return App::Startup();
	}

	void MyApp::Shutdown()
	{
		// DisplaySubsystem이 닫힌 창을 먼저 해제할 수 있으므로 종료 후 창 포인터를 사용하지 않습니다.
		window = nullptr;
		App::Shutdown();
	}
}
