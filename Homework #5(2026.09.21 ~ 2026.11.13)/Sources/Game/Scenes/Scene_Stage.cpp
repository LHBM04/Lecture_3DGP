#include "Precompiled.hpp"
#include "Scene_Stage.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

#include "Framework/Core.hpp"
#include "Framework/GameObjects.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"

#include "Framework/UI/ButtonUI.hpp"
#include "Framework/UI/ImageView.hpp"

#include "../Core/Input.hpp"
#include "../Core/Time.hpp"

#include "../Components/PlayerController.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	void Scene_Stage::OnEnter()
	{
		assets = App::GetInstance().GetSubsystem<AssetSubsystem>();
		if (!assets)
		{
			Logger::LogCritical("에셋 서브시스템을 사용할 수 없습니다.");
			return;
		}

		// 카메라 세팅
		{
			GameObject* cameraGameObject = Instantiate();
			cameraGameObject->SetName("Camera");
			cameraGameObject->SetTag("Main Camera");

			Transform* cameraTransform = cameraGameObject->AddComponent<Transform>();
			cameraTransform->SetLocalRotation(Quaternion<f32>{});
			cameraTransform->SetLocalPosition(Vector3D<f32>(0.0f, 0.0f, -10.0f));

			Camera* camera = cameraGameObject->AddComponent<Camera>();
			camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, 1280.0f, 960.0f));
			camera->SetScissorRect(CD3DX12_RECT(0, 0, 1280, 960));
		}

		// 스카이박스 세팅
		{
			GameObject* skybox = Instantiate();
			skybox->SetName("Skybox");

			SkyboxRenderer* skyboxRenderer = skybox->AddComponent<SkyboxRenderer>();
			skyboxRenderer->SetCubemap(assets->LoadAsset<CubemapAsset>("Resources/Cubemaps/Polyverse Skies - Blue Sky/Polyverse Skies - Blue Sky.dds"));
			skyboxRenderer->SetMaterial(assets->LoadAsset<MaterialAsset>("Resources/Materials/Skybox.tukmat"));
		}

		// 플레이어 세팅
		{
			GameObject* tank = Instantiate(assets->LoadAsset<ModelAsset>("Resources/Models/Carro_Armato_L6_40.tukmodel"));
			tank->SetName("Tank");

			Transform* tankTransform = tank->AddComponent<Transform>();
			tankTransform->SetLocalPosition(Vector3D<f32>(0.0f, 0.0f, 0.0f));
			tankTransform->SetLocalRotation(Quaternion<f32>{});
			tankTransform->SetLocalScale(Vector3D<f32>(1.0f, 1.0f, 1.0f));

			PlayerController* playerController = tank->AddComponent<PlayerController>();
		}
	}

	void Scene_Stage::OnUpdate()
	{
		if (Input::IsKeyPressed(KeyCode::ESCAPE))
		{
			App::GetInstance().GetSubsystem<SceneSubsystem>()->ExitScene();
		}
	}
}
