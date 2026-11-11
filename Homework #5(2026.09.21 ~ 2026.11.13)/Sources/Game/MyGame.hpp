#pragma once

#include "Framework/Core.hpp"
#include "Framework/Platform.hpp"
#include "Framework/Objects.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"

#include "WorldTransition.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class MyGame : public App
	{
		using App::App;

	public:
		SystemStatus Startup() override;
		SystemStatus Routine() override;
		void Shutdown() override;

	private:
		DisplaySubsystem* displaySubsystem{ nullptr };
		RenderSubsystem* renderSubsystem{ nullptr };
		WorldSubsystem* worldSubsystem{ nullptr };
		InputSubsystem* inputSubsystem{ nullptr };
		AssetSubsystem* assetSubsystem{ nullptr };
		WorldTransition worldTransition;
		Delegate<GPUCommandBuffer&>::ListenerId transitionRenderListenerId = 0;
	};
}
