#include "Precompiled.hpp"
#include "TestWorld.hpp"

#include <iostream>

#include "Framework/Resources.hpp"

#include "Framework/Core/App.hpp"

#include "Framework/Entities/Entity.hpp"
#include "Framework/Entities/Transform.hpp"

#include "Framework/Graphics/Camera.hpp"
#include "Framework/Graphics/MeshRenderer.hpp"

namespace TUK::Game
{
	void TestWorld::OnEnter()
	{
		using namespace Framework;

		Entity& cameraEntity = CreateEntity("Camera", "Camera");
		cameraEntity.AddComponent<Transform>()->SetLocalPosition(Vector3D<float>(0.0f, 0.0f, -2.0f));
		Camera* camera = cameraEntity.AddComponent<Camera>();
		camera->SetViewport(D3D12_VIEWPORT{ 0.0f, 0.0f, 1280.0f, 960.0f, 0.0f, 1.0f });
		camera->SetScissorRect(D3D12_RECT{ 0, 0, 1280, 960 });

		AssetSubsystem* assets = App::GetInstance()->GetSubsystem<AssetSubsystem>();
		if (!assets)
		{
			std::cerr << "TestWorld: AssetSubsystem이 없습니다." << std::endl;
			return;
		}

		ModelAsset* model = assets->LoadAsset<ModelAsset>("Resources\\Models\\Spaceship.bin");
		if (!model)
		{
			std::cerr << "TestWorld: 장면 에셋 로딩에 실패했습니다." << std::endl;
			return;
		}

		Entity& modelEntity = CreateEntity("Spaceship", "Untagged");
		modelEntity.AddComponent<Transform>()->SetLocalScale(Vector3D<float>(0.01f, 0.01f, 0.01f));
		MeshRenderer* meshRenderer = modelEntity.AddComponent<MeshRenderer>();
		meshRenderer->SetModel(model);
	}
}
