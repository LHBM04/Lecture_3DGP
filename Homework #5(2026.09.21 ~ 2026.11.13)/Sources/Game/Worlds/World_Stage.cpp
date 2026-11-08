#include "Precompiled.hpp"

#include <filesystem>
#include <iostream>
#include "World_Stage.hpp"

#include "Framework/Core.hpp"
#include "Framework/Entities.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"
#include "Framework/Platform/InputSubsystem.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	void World_Stage::OnEnter()
	{
		assets = App::GetInstance().GetSubsystem<AssetSubsystem>();
		if (!assets)
		{
			std::cerr << "World_Stage: AssetSubsystem is not available." << std::endl;
			return;
		}

		input = App::GetInstance().GetSubsystem<InputSubsystem>();
		if (!input)
		{
			std::cerr << "World_Stage: InputSubsystem is not available." << std::endl;
			return;
		}

		Entity* cameraEntity = Instantiate();
		cameraEntity->SetName("Camera");
		cameraEntity->SetTag("Main Camera");
		cameraEntity->AddComponent<Transform>()->SetLocalPosition(Vector3D<f32>(0.0f, 10.0f, -100.0f));

		Camera* camera = cameraEntity->AddComponent<Camera>();
		camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, 1280.0f, 960.0f));
		camera->SetScissorRect(CD3DX12_RECT(0, 0, 1280, 960));

		const std::filesystem::path MODEL_PATH = "Resources/Models/Target.bin";
		if (const ModelAsset* model = assets->LoadAsset<ModelAsset>(MODEL_PATH))
		{
			if (!Instantiate(model, Vector3D<f32>(0.0f, 0.0f, 0.0f)))
			{
				return;
			}
		}
	}

	void World_Stage::OnUpdate()
	{
		
	}
}
