#include "Precompiled.hpp"
#include "Scene_LevelFail.hpp"

#include "Framework/Core.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Platform.hpp"
#include "Framework/Resources.hpp"
#include "Framework/UI.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Input.hpp"
#include "../Core/SceneManager.hpp"

namespace TUK::Game
{
	void Scene_LevelFail::OnEnter()
	{
		{
			GameObject* cameraGameObject = Instantiate();
			cameraGameObject->SetName("Camera");
			cameraGameObject->SetTag("Main Camera");

			Transform* cameraTransform = cameraGameObject->AddComponent<Transform>();
			cameraTransform->SetLocalRotation(Quaternion<f32>{});
			cameraTransform->SetLocalPosition(Vector3D<f32>(0.0f, 0.0f, -10.0f));

			const DisplaySubsystem* displays = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
			ASSERT_FMT(displays, "디스플레이 서브시스템을 가져오지 못했습니다.");

			const Display* display = displays->GetDisplay(0);
			ASSERT_FMT(display, "게임 오버 화면을 표시할 디스플레이가 없습니다.");

			const Vector2D<i32> clientSize = display->GetClientSize();
			Camera* camera = cameraGameObject->AddComponent<Camera>();
			camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<f32>(clientSize.GetX()), static_cast<f32>(clientSize.GetY())));
			camera->SetScissorRect(CD3DX12_RECT(0, 0, clientSize.GetX(), clientSize.GetY()));
			camera->SetClearColor(ColorRGBA<f32>::GetBlack());
		}

		{
			GameObject* titleGameObject = Instantiate();
			titleGameObject->SetName("Game Over Text");

			Transform* titleTransform = titleGameObject->AddComponent<Transform>();
			titleTransform->SetLocalPosition({ 0.0f, -48.0f, 0.0f });
			titleTransform->SetLocalRotation(Quaternion<f32>{});
			titleTransform->SetLocalScale({ 400.0f, 72.0f, 1.0f });

			TextView* titleText = titleGameObject->AddComponent<TextView>();
			titleText->SetHorizontalAlignment(TextHorizontalAlignment::CENTER);
			titleText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
			titleText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
			titleText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
			titleText->SetText("GAME OVER");
			titleText->SetFontSize(60.0f);
			titleText->SetColor(ColorRGBA<f32>::GetWhite());
		}

		{
			GameObject* returnGameObject = Instantiate();
			returnGameObject->SetName("Return To Title Button");

			Transform* returnTransform = returnGameObject->AddComponent<Transform>();
			returnTransform->SetLocalPosition({ 0.0f, 56.0f, 0.0f });
			returnTransform->SetLocalRotation(Quaternion<f32>{});
			returnTransform->SetLocalScale({ 400.0f, 56.0f, 1.0f });

			TextView* returnText = returnGameObject->AddComponent<TextView>();
			returnText->SetHorizontalAlignment(TextHorizontalAlignment::CENTER);
			returnText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
			returnText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
			returnText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
			returnText->SetText("RETURN TO TITLE");
			returnText->SetFontSize(36.0f);
			returnText->SetColor(ColorRGBA<f32>::GetWhite());

			ButtonUI* returnButton = returnGameObject->AddComponent<ButtonUI>();
			returnButton->OnEnter() += [returnText]()
			{
				returnText->SetColor(ColorRGBA<f32>::GetYellow());
			};
			returnButton->OnExit() += [returnText]()
			{
				returnText->SetColor(ColorRGBA<f32>::GetWhite());
			};
			returnButton->OnClick() += []()
			{
				SceneManager::LoadScene("Title");
			};
		}

		if (InputAction* cancelAction = Input::GetAction("Cancel"))
		{
			returnListenerId = cancelAction->OnStarted().AddListener([](const InputActionContext&)
			{
				SceneManager::LoadScene("Title");
			});
		}
	}

	void Scene_LevelFail::OnExit()
	{
		if (returnListenerId != -1)
		{
			if (InputAction* cancelAction = Input::GetAction("Cancel"))
			{
				cancelAction->OnStarted().RemoveListener(returnListenerId);
			}

			returnListenerId = -1;
		}
	}
}
