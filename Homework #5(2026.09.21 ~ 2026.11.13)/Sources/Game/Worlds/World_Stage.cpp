#include "Precompiled.hpp"
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

		Entity& cameraEntity = Instantiate("Camera", "Main Camera");
		cameraEntity.AddComponent<Transform>()->SetLocalPosition(Vector3D<f32>(0.0f, 10.0f, -100.0f));

		Camera* camera = cameraEntity.AddComponent<Camera>();
		camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, 1280.0f, 960.0f));
		camera->SetScissorRect(CD3DX12_RECT(0, 0, 1280, 960));

		Entity& entity = Instantiate("Cube", "Cube");
		entity.AddComponent<Transform>()->SetLocalPosition(Vector3D<f32>(0.0f, 0.0f, 0.0f));

		MeshRenderer* renderer = entity.AddComponent<MeshRenderer>();
		if (const ModelAsset* model = assets->LoadAsset<ModelAsset>("Resources\\Models\\Spaceship.bin"))
		{
			renderer->SetModel(*model);
		}

		if (const MaterialAsset* material = assets->LoadAsset<MaterialAsset>("Resources\\Materials\\Collider_WeaponsSupport_Collider_WeaponsSupport_Lit.bin"))
		{
			renderer->SetMaterial(*material);
		}
	}

	void World_Stage::OnUpdate()
	{
		
	}
}
