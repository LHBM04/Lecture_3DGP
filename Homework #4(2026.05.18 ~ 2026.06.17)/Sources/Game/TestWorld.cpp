#include "Precompiled.hpp"
#include "TestWorld.hpp"

#include "Framework/Resources.hpp"

#include "Framework/Core/App.hpp"

#include "Framework/Entities/Entity.hpp"
#include "Framework/Entities/Transform.hpp"

#include "Framework/Graphics/Camera.hpp"
#include "Framework/Graphics/MeshRenderer.hpp"

#include "Input.hpp"
#include "Time.hpp"
#include "Framework/Entities/WorldSubsystem.hpp"

namespace TUK::Game
{
	namespace
	{
		Framework::Entity* modelEntity = nullptr;
	}

	void TestWorld::OnEnter()
	{
		using namespace Framework;

		Entity& cameraEntity = CreateEntity("Camera", "Main Camera");
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

		modelEntity = &CreateEntity("Spaceship", "Untagged");
		modelEntity->AddComponent<Transform>()->SetLocalScale(Vector3D<float>(0.01f, 0.01f, 0.01f));

		MeshRenderer* meshRenderer = modelEntity->AddComponent<MeshRenderer>();
		meshRenderer->SetModel(model);
	}

	void TestWorld::OnUpdate()
	{
		if (Input::IsKeyPressed(VK_ESCAPE))
		{
			Framework::WorldSubsystem* worldSubsystem = Framework::App::GetInstance()->GetSubsystem<Framework::WorldSubsystem>();
			if (worldSubsystem)
			{
				worldSubsystem->ExitWorld();
				return;
			}
		}

		if (Input::IsKeyHeld('W'))
		{
			auto currentPos = modelEntity->GetComponent<Framework::Transform>()->GetWorldPosition();
			auto nextPos = currentPos + Framework::Vector3D<float>::GetForward() * 10.0f * Time::GetDeltaTime();
			modelEntity->GetComponent<Framework::Transform>()->SetWorldPosition(nextPos);
		}
		if (Input::IsKeyHeld('A'))
		{
			auto currentPos = modelEntity->GetComponent<Framework::Transform>()->GetWorldPosition();
			auto nextPos = currentPos + Framework::Vector3D<float>::GetLeft() * 10.0f * Time::GetDeltaTime();
			modelEntity->GetComponent<Framework::Transform>()->SetWorldPosition(nextPos);
		}
		if (Input::IsKeyHeld('S'))
		{
			auto currentPos = modelEntity->GetComponent<Framework::Transform>()->GetWorldPosition();
			auto nextPos = currentPos + Framework::Vector3D<float>::GetBack() * 10.0f * Time::GetDeltaTime();
			modelEntity->GetComponent<Framework::Transform>()->SetWorldPosition(nextPos);
		}
		if (Input::IsKeyHeld('D'))
		{
			auto currentPos = modelEntity->GetComponent<Framework::Transform>()->GetWorldPosition();
			auto nextPos = currentPos + Framework::Vector3D<float>::GetRight() * 10.0f * Time::GetDeltaTime();
			modelEntity->GetComponent<Framework::Transform>()->SetWorldPosition(nextPos);
		}
		if (Input::IsKeyHeld(VK_SPACE))
		{
			auto currentPos = modelEntity->GetComponent<Framework::Transform>()->GetWorldPosition();
			auto nextPos = currentPos + Framework::Vector3D<float>::GetUp() * 10.0f * Time::GetDeltaTime();
			modelEntity->GetComponent<Framework::Transform>()->SetWorldPosition(nextPos);
		}
		if (Input::IsKeyHeld(VK_CONTROL))
		{
			auto currentPos = modelEntity->GetComponent<Framework::Transform>()->GetWorldPosition();
			auto nextPos = currentPos + Framework::Vector3D<float>::GetDown() * 10.0f * Time::GetDeltaTime();
			modelEntity->GetComponent<Framework::Transform>()->SetWorldPosition(nextPos);
		}
	}
}
