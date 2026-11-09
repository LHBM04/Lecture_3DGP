#include "Precompiled.hpp"
#include "World_Stage.hpp"

#include "Framework/Core.hpp"
#include "Framework/Entities.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"
#include "Framework/Platform/InputSubsystem.hpp"

#include "../Input.hpp"
#include "../Time.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	void World_Stage::OnEnter()
	{
		assets = App::GetInstance().GetSubsystem<AssetSubsystem>();
		if (!assets)
		{
			Logger::LogCritical("에셋 서브시스템을 사용할 수 없습니다.");
			return;
		}

		input = App::GetInstance().GetSubsystem<InputSubsystem>();
		if (!input)
		{
			Logger::LogCritical("입력 서브시스템을 사용할 수 없습니다.");
			return;
		}

		Entity* cameraEntity = Instantiate();
		cameraEntity->SetName("Camera");
		cameraEntity->SetTag("Main Camera");
		cameraEntity->AddComponent<Transform>()->SetLocalPosition(Vector3D<f32>(0.0f, 0.0f, -10.0f));

		Camera* camera = cameraEntity->AddComponent<Camera>();
		camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, 1280.0f, 960.0f));
		camera->SetScissorRect(CD3DX12_RECT(0, 0, 1280, 960));

		if (const ModelAsset* model = assets->LoadAsset<ModelAsset>("Resources/Models/Model.json"))
		{
			tank = Instantiate(model, Vector3D<f32>(0.0f, 0.0f, 0.0f));
		}
	}

	void World_Stage::OnUpdate()
	{
		Transform* transform = tank->GetComponent<Transform>();
		if (!transform)
		{
			Logger::LogError("유효한 Transform 컴포넌트를 찾을 수 없습니다.");
		}

		const Quaternion<f32> currentRotation = transform->GetLocalRotation();
		Quaternion<f32> nextRotation = currentRotation;

		if (Input::IsKeyHeld(KeyCode::LEFT))
		{
			nextRotation *= Quaternion<f32>::Euler(0.0f, -0.1f, 0.0f) * 10.0f * Time::GetDeltaTime();
		}
		if (Input::IsKeyHeld(KeyCode::RIGHT))
		{
			nextRotation *= Quaternion<f32>::Euler(0.0f, 0.1f, 0.0f) * 10.0f * Time::GetDeltaTime();
		}

		transform->SetLocalRotation(nextRotation);
	}
}
