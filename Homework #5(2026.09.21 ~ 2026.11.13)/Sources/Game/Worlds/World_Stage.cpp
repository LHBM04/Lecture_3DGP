#include "Precompiled.hpp"
#include "World_Stage.hpp"

#include <algorithm>
#include <cmath>

#include "Framework/Core.hpp"
#include "Framework/Objects.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"

#include "Framework/UI/ButtonUI.hpp"
#include "Framework/UI/ImageView.hpp"

#include "../Components/PlayerController.hpp"
#include "../Input.hpp"
#include "../Time.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	void World_Stage::OnEnter()
	{
		cameraTransform = nullptr;
		cameraYaw = 0.0f;
		cameraPitch = 0.0f;

		assets = App::GetInstance().GetSubsystem<AssetSubsystem>();
		if (!assets)
		{
			Logger::LogCritical("에셋 서브시스템을 사용할 수 없습니다.");
			return;
		}

		GameObject* cameraGameObject = Instantiate();
		cameraGameObject->SetName("Camera");
		cameraGameObject->SetTag("Main Camera");
		cameraTransform = cameraGameObject->AddComponent<Transform>();
		cameraTransform->SetLocalRotation(Quaternion<f32>{});
		cameraTransform->SetLocalPosition(Vector3D<f32>(0.0f, 0.0f, -10.0f));

		Camera* camera = cameraGameObject->AddComponent<Camera>();
		camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, 1280.0f, 960.0f));
		camera->SetScissorRect(CD3DX12_RECT(0, 0, 1280, 960));

		if (const ModelAsset* model = assets->LoadAsset<ModelAsset>("Resources/Models/Chi_Ha/Chi_Ha.tukmodel"))
		{
			tank = Instantiate(model, Vector3D<f32>(0.0f, 0.0f, 0.0f));
			tank->AddComponent<PlayerController>();
		}

		camera->SetClearFlags(CameraClearFlags::Skybox);
		camera->SetCubemap(assets->LoadAsset<Cubemap>("Resources/Textures/Polyverse Skies - Blue Sky.dds"));
		SetSkyboxMaterial(assets->LoadAsset<MaterialAsset>("Resources/Materials/Skybox.tukmaterial"));

		auto test = Instantiate(Vector3D<f32>::GetZero());
		test->GetComponent<Transform>()->SetLocalScale(Vector3D<f32>(100.0f, 100.0f, 1.0f));
		test->AddComponent<ImageView>()->SetImage(assets->LoadAsset<TextureAsset>("Resources/Textures/Test.dds"));

		auto buttonTest = test->AddComponent<ButtonUI>();
		buttonTest->OnEnter() += []()
		{
			LOG_INFO("마우스가 버튼 위에 올라갔습니다.");
		};
		buttonTest->OnExit() += []()
		{
			LOG_INFO("마우스가 버튼에서 벗어났습니다.");
		};
		buttonTest->OnClick() += []()
		{
			LOG_INFO("버튼이 클릭되었습니다.");
		};
	}

	void World_Stage::OnUpdate()
	{
		if (Input::IsKeyPressed(KeyCode::ESCAPE))
		{
			App::GetInstance().GetSubsystem<WorldSubsystem>()->ExitWorld();
			return;
		}

		if (!cameraTransform)
		{
			return;
		}

		constexpr f32 CAMERA_MOVE_SPEED = 8.0f;
		constexpr f32 MOUSE_SENSITIVITY = 0.12f;
		constexpr f32 MAX_PITCH = 89.0f;
		constexpr f32 FULL_TURN = 360.0f;

		const Vector2D<i32> mouseDelta = Input::GetMousePositionDelta();
		if (Input::IsMouseButtonHeld(ButtonCode::LEFT) && (mouseDelta.GetX() != 0 || mouseDelta.GetY() != 0))
		{
			cameraYaw = std::remainder(cameraYaw + static_cast<f32>(mouseDelta.GetX()) * MOUSE_SENSITIVITY, FULL_TURN);
			cameraPitch = std::clamp(cameraPitch + static_cast<f32>(mouseDelta.GetY()) * MOUSE_SENSITIVITY,
				-MAX_PITCH, MAX_PITCH);
			cameraTransform->SetLocalRotation(Quaternion<f32>::Euler(cameraPitch, cameraYaw, 0.0f));
		}

		const f32 forwardInput = static_cast<f32>(Input::IsKeyHeld(KeyCode::W)) -
			static_cast<f32>(Input::IsKeyHeld(KeyCode::S));
		const f32 rightInput = static_cast<f32>(Input::IsKeyHeld(KeyCode::D)) -
			static_cast<f32>(Input::IsKeyHeld(KeyCode::A));
		if (forwardInput == 0.0f && rightInput == 0.0f)
		{
			return;
		}

		const Matrix4x4<f32>& cameraMatrix = cameraTransform->GetWorldMatrix();
		const Vector3D<f32> direction = cameraMatrix.GetForward() * forwardInput + cameraMatrix.GetRight() * rightInput;
		const Vector3D<f32> displacement = direction.GetNormalized() * (CAMERA_MOVE_SPEED * Time::GetDeltaTime());
		cameraTransform->SetWorldPosition(cameraTransform->GetWorldPosition() + displacement);
	}
}
