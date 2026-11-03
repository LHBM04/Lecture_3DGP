#include "Precompiled.hpp"
#include "World_Stage.hpp"

#include "Framework/Core.hpp"
#include "Framework/Entities.hpp"
#include "Framework/Graphics.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	void World_Stage::OnEnter()
	{
		Entity& cameraEntity = Instantiate("Camera", "Main Camera");
		cameraEntity.AddComponent<Transform>()->SetLocalPosition(Vector3D<f32>(0.0f, 0.0f, -2.0f));

		Camera* camera = cameraEntity.AddComponent<Camera>();
		camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, 1280.0f, 960.0f));
		camera->SetScissorRect(CD3DX12_RECT(0, 0, 1280, 960));
	}

	void World_Stage::OnUpdate()
	{

	}
}
