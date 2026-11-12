#include "Precompiled.hpp"
#include "Scene_LevelTank.hpp"

#include "Framework/Core.hpp"
#include "Framework/Objects.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"

#include "Framework/UI/ButtonUI.hpp"
#include "Framework/UI/ImageView.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Input.hpp"
#include "../Core/Time.hpp"

#include "../Components/PlayerTankController.hpp"

namespace TUK::Game
{
	void Scene_LevelTank::OnEnter()
	{
		// 터레인 세팅
		Terrain* terrain = nullptr;
		{
			GameObject* terrainGameObject = Instantiate();
			terrainGameObject->SetName("Terrain");
			terrainGameObject->SetTag("Terrain");

			Transform* terrainTransform = terrainGameObject->AddComponent<Transform>();
			terrainTransform->SetLocalPosition(Vector3D<f32>::GetZero());
			terrainTransform->SetLocalRotation({});
			terrainTransform->SetLocalScale(Vector3D<f32>::GetOne());

			terrain = terrainGameObject->AddComponent<Terrain>();
			terrain->SetTerrain(Assets::LoadAsset<TerrainAsset>("Resources/Terrains/HeightMap0.raw"));

			MeshRenderer* terrainRenderer = terrainGameObject->AddComponent<MeshRenderer>();
			terrainRenderer->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/Terrain.tukmat"));
		}

		const f32 terrainHeight = (terrain && terrain->IsValid())
			? terrain->SampleHeightAtWorldPosition(Vector3D<f32>::GetZero())
			: 0.0f;

		// 카메라 세팅
		{
			GameObject* cameraGameObject = Instantiate();
			cameraGameObject->SetName("Camera");
			cameraGameObject->SetTag("Main Camera");

			Transform* cameraTransform = cameraGameObject->AddComponent<Transform>();
			cameraTransform->SetLocalRotation(Quaternion<f32>{});
			cameraTransform->SetLocalPosition(Vector3D<f32>(0.0f, terrainHeight + 3.0f, -10.0f));

			const DisplaySubsystem* displays = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
			ASSERT_FMT(displays, "디스플레이 서브시스템을 가져오지 못했습니다.");

			const Display* display = displays->GetDisplay(0);
			ASSERT_FMT(display, "타이틀 화면을 표시할 디스플레이가 없습니다.");

			const Vector2D<i32> clientSize = display->GetClientSize();
			Camera* camera = cameraGameObject->AddComponent<Camera>();
			camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<f32>(clientSize.GetX()), static_cast<f32>(clientSize.GetY())));
			camera->SetScissorRect(CD3DX12_RECT(0, 0, clientSize.GetX(), clientSize.GetY()));
			camera->SetClearColor(ColorRGBA<f32>::GetBlue());
		}

		// 스카이박스 세팅
		{
			// 낮의 경우
			GameObject* skybox = Instantiate();
			skybox->SetName("Skybox Day");
			skybox->SetTag("Skybox");

			SkyboxRenderer* skyboxRenderer = skybox->AddComponent<SkyboxRenderer>();
			skyboxRenderer->SetCubemap(Assets::LoadAsset<CubemapAsset>("Resources/Cubemaps/Polyverse Skies - Blue Sky/Polyverse Skies - Blue Sky.dds"));
			skyboxRenderer->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/Skybox.tukmat"));
		}
		// 플레이어 세팅
		{
			GameObject* playerObject = Instantiate(Assets::LoadAsset<ModelAsset>("Resources/Models/Carro_Armato_L6_40.tukmodel"));
			playerObject->SetName("Tank");
			playerObject->SetTag("Player");

			Transform* playerTransform = playerObject->AddComponent<Transform>();
			playerTransform->SetLocalPosition(Vector3D<f32>(0.0f, terrainHeight, 0.0f));
			playerTransform->SetLocalRotation(Quaternion<f32>{});
			playerTransform->SetLocalScale(Vector3D<f32>(1.0f, 1.0f, 1.0f));

			// 나중에 쓸 일 있으면 어트리뷰트 떼고 쓰자
			[[maybe_unused]] PlayerTankController* playerController = playerObject->AddComponent<PlayerTankController>();
		}
		// 입력 액션 세팅
		if (InputAction* cancelAction = Input::GetAction("Cancel"))
		{
			cancelListenerID = cancelAction->OnStarted().AddListener([](const InputActionContext&)
			{
				// 씬 탈출 (다음 이동 씬이 없기 때문에 애플리케이션 종료)
				App::GetInstance().GetSubsystem<SceneSubsystem>()->ExitScene();
			});
		}
	}

	void Scene_LevelTank::OnExit()
	{
		if (cancelListenerID != 0)
		{
			if (InputAction* cancelAction = Input::GetAction("Cancel"))
			{
				cancelAction->OnStarted().RemoveListener(cancelListenerID);
			}

			cancelListenerID = 0;
		}
	}
}
