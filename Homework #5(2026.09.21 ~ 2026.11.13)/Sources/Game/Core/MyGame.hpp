#pragma once

#include "Framework/Core.hpp"
#include "Framework/Platform.hpp"
#include "Framework/GameObjects.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class MyGame : public App
	{
		using App::App;

	public:
		SystemStatus Startup() override;
		SystemStatus Routine() override;
		void Shutdown() override;

	private:
		void OnInputSubsystemAdded(InputSubsystem* subsystem);
		void OnInputSubsystemRemoved(InputSubsystem* subsystem);
		void OnSceneSubsystemAdded(SceneSubsystem* subsystem);
		void OnSceneSubsystemRemoved(SceneSubsystem* subsystem);
		void OnAssetSubsystemAdded(AssetSubsystem* subsystem);
		void OnAssetSubsystemRemoved(AssetSubsystem* subsystem);

		EventSubsystem* eventSubsystem = nullptr;
		DisplaySubsystem* displaySubsystem = nullptr;
		RenderSubsystem* renderSubsystem = nullptr;
		SceneSubsystem* sceneSubsystem = nullptr;
		InputSubsystem* inputSubsystem = nullptr;
		AssetSubsystem* assetSubsystem = nullptr;
	};
}
