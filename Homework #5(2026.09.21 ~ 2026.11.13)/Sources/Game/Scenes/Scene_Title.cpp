#include "Precompiled.hpp"
#include "Scene_Title.hpp"

#include "Framework/Core.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Platform/DisplaySubsystem.hpp"
#include "Framework/Resources.hpp"
#include "Framework/UI.hpp"

#include "../Components/TitleAnimationSequence.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Input.hpp"
#include "../Core/SceneManager.hpp"

namespace TUK::Game
{
	void Scene_Title::OnEnter()
	{
		GameObject* cameraTarget = Instantiate(Vector3D<f32>::GetZero());
		cameraTarget->SetName("Title Camera Target");
		TitleAnimationSequence* sequence = nullptr;

		// 카메라 세팅
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
			ASSERT_FMT(display, "타이틀 화면을 표시할 디스플레이가 없습니다.");

			const Vector2D<i32> clientSize = display->GetClientSize();

			Camera* camera = cameraGameObject->AddComponent<Camera>();
			camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<f32>(clientSize.GetX()), static_cast<f32>(clientSize.GetY())));
			camera->SetScissorRect(CD3DX12_RECT(0, 0, clientSize.GetX(), clientSize.GetY()));
			camera->SetClearColor(ColorRGBA<f32>::GetBlue());

			sequence = cameraGameObject->AddComponent<TitleAnimationSequence>();
			sequence->SetTarget(cameraTarget);
		}
		// 스카이박스 세팅
		{
			// 낮의 경우
			{
				GameObject* skybox = Instantiate();
				skybox->SetName("Skybox Day");
				skybox->SetTag("Skybox");

				SkyboxRenderer* skyboxRenderer = skybox->AddComponent<SkyboxRenderer>();
				skyboxRenderer->SetCubemap(Assets::LoadAsset<CubemapAsset>("Resources/Cubemaps/Polyverse Skies - Blue Sky/Polyverse Skies - Blue Sky.dds"));
				skyboxRenderer->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/Skybox.tukmat"));
			}
			// 밤의 경우
			{
				GameObject* skybox = Instantiate();
				skybox->SetName("Skybox Night");
				skybox->SetTag("Skybox");

				SkyboxRenderer* nightSkybox = skybox->AddComponent<SkyboxRenderer>();
				nightSkybox->SetCubemap(Assets::LoadAsset<CubemapAsset>("Resources/Cubemaps/Polyverse Skies - Night Sky/Polyverse Skies - Night Sky.dds"));
				nightSkybox->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/Skybox.tukmat"));
				sequence->SetNightSkybox(nightSkybox);
			}
		}
		// 타이틀 로고 세팅
		{
			// 로고 세팅
			{
				GameObject* logoGameObject = Instantiate();
				logoGameObject->SetName("Title Logo");

				Transform* logoTransform = logoGameObject->AddComponent<Transform>();
				logoTransform->SetLocalPosition({ 0.0f, -200.0f, 0.0f });
				logoTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
				logoTransform->SetLocalScale({ 780.0f, 80.0f, 1.0f });

				TextView* titleText = logoGameObject->AddComponent<TextView>();
				titleText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
				titleText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
				titleText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
				titleText->SetText("3D GAME PROGRAMMING");
				titleText->SetFontSize(64.0f);
			}
			// 부제 세팅
			{
				GameObject* subtitleGameObject = Instantiate();
				subtitleGameObject->SetName("Title Subtitle");

				Transform* subtitleTransform = subtitleGameObject->AddComponent<Transform>();
				subtitleTransform->SetLocalPosition({ 270.0f, -140.0f, 0.0f });
				subtitleTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
				subtitleTransform->SetLocalScale({ 240.0f, 40.0f, 1.0f });

				TextView* subtitleText = subtitleGameObject->AddComponent<TextView>();
				subtitleText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
				subtitleText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
				subtitleText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
				subtitleText->SetText("1st Homework");
				subtitleText->SetFontSize(32.0f);
			}
		}
		// 시작 버튼 세팅
		{
			GameObject* startGameObject = Instantiate();
			startGameObject->SetName("Start Button");

			Transform* startTransform = startGameObject->AddComponent<Transform>();
			startTransform->SetLocalPosition({ 0.0f, 80.0f, 0.0f });
			startTransform->SetLocalRotation({0.0f, 0.0f, 0.0f, 1.0f});
			startTransform->SetLocalScale({ 136.0f, 56.0f, 1.0f });

			TextView* startText = startGameObject->AddComponent<TextView>();
			startText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
			startText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
			startText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
			startText->SetText("START");
			startText->SetFontSize(40.0f);

			ButtonUI* startButton = startGameObject->AddComponent<ButtonUI>();
#ifdef _DEBUG
			startButton->SetDebugMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/DebugUI.tukmat"));
#endif
			startButton->OnEnter() += [startText]() {
				startText->SetColor(ColorRGBA<f32>::GetYellow());
			};
			startButton->OnExit() += [startText]() {
				startText->SetColor(ColorRGBA<f32>::GetWhite());
			};
			startButton->OnClick() += []()
			{
				SceneManager::LoadScene("Stage Tank");
			};
		}
		// 종료 버튼 세팅
		{
			GameObject* exitGameObject = Instantiate();
			exitGameObject->SetName("Exit Button");

			Transform* exitTransform = exitGameObject->AddComponent<Transform>();
			exitTransform->SetLocalPosition({ 0.0f, 180.0f, 0.0f });
			exitTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
			exitTransform->SetLocalScale({ 92.0f, 56.0f, 1.0f });

			TextView* exitText = exitGameObject->AddComponent<TextView>();
			exitText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
			exitText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
			exitText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
			exitText->SetText("EXIT");
			exitText->SetFontSize(40.0f);

			ButtonUI* exitButton = exitGameObject->AddComponent<ButtonUI>();
#ifdef _DEBUG
			exitButton->SetDebugMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/DebugUI.tukmat"));
#endif
			exitButton->OnEnter() += [exitText]() {
				exitText->SetColor(ColorRGBA<f32>::GetYellow());
			};
			exitButton->OnExit() += [exitText]() {
				exitText->SetColor(ColorRGBA<f32>::GetWhite());
			};
			exitButton->OnClick() += []()
			{
				SceneManager::LoadScene("Stage");
			};
		}
	}

}
