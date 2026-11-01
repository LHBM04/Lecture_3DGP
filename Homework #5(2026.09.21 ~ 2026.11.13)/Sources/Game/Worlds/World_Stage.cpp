#include "Precompiled.hpp"
#include "World_Stage.hpp"

#include "Framework/Core.hpp"
#include "Framework/Platform.hpp"

#include "Framework/Entities/Entity.hpp"
#include "Framework/Entities/Transform.hpp"
#include "Framework/Entities/Component.hpp"

#include "Framework/Graphics.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	void World_Stage::OnEnter()
	{
		Entity& cameraEntity = CreateEntity("Camera", "Main Camera");
		cameraEntity.AddComponent<Transform>()->SetLocalPosition(Vector3D<float>(0.0f, 0.0f, -2.0f));

		Camera* camera = cameraEntity.AddComponent<Camera>();
		camera->SetViewport(D3D12_VIEWPORT{ 0.0f, 0.0f, 1280.0f, 960.0f, 0.0f, 1.0f });
		camera->SetScissorRect(D3D12_RECT{ 0, 0, 1280, 960 });
	}

	void World_Stage::OnUpdate()
	{

	}
}
