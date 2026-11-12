#include "Precompiled.hpp"
#include "Scene_Title.hpp"

#include "Framework/Core.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Platform.hpp"
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
		TitleAnimationSequence* sequence = nullptr;

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

			GameObject* cameraTarget = Instantiate(Vector3D<f32>::GetZero());
			cameraTarget->SetName("Title Camera Target");

			sequence = cameraGameObject->AddComponent<TitleAnimationSequence>();
			sequence->SetTarget(cameraTarget);
		}

		{
			{
				GameObject* skybox = Instantiate();
				skybox->SetName("Skybox Day");
				skybox->SetTag("Skybox");

				SkyboxRenderer* skyboxRenderer = skybox->AddComponent<SkyboxRenderer>();
				skyboxRenderer->SetCubemap(Assets::LoadAsset<CubemapAsset>("Resources/Cubemaps/Polyverse Skies - Blue Sky/Polyverse Skies - Blue Sky.dds"));
				skyboxRenderer->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/Skybox.tukmat"));
			}

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

		{
			GameObject* lightGameObject = Instantiate();
			lightGameObject->SetName("Directional Light");
			lightGameObject->SetTag("Light");

			Transform* lightTransform = lightGameObject->AddComponent<Transform>();
			lightTransform->SetLocalPosition(Vector3D<f32>(0.0f, 50.0f, 0.0f));
			lightTransform->SetLocalRotation(Quaternion<f32>::Euler(50.0f, -30.0f, 0.0f));

			DirectionalLight* light = lightGameObject->AddComponent<DirectionalLight>();
			light->SetColor(ColorRGBA<f32>::GetWhite());
			light->SetIntensity(1.0f);
			light->SetAmbientColor(ColorRGBA<f32>(0.20f, 0.20f, 0.25f, 1.0f));
		}

		{
			{
				GameObject* logoGameObject = Instantiate();
				logoGameObject->SetName("Title Logo");

				Transform* logoTransform = logoGameObject->AddComponent<Transform>();
				logoTransform->SetLocalPosition({ 0.0f, -145.0f, 0.0f });
				logoTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
				logoTransform->SetLocalScale({ 588.0f, 60.0f, 1.0f });

				TextView* titleText = logoGameObject->AddComponent<TextView>();
				titleText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
				titleText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
				titleText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
				titleText->SetText("3D GAME PROGRAMMING");
				titleText->SetFontSize(48.0f);
			}

			{
				GameObject* subtitleGameObject = Instantiate();
				subtitleGameObject->SetName("Title Subtitle");

				Transform* subtitleTransform = subtitleGameObject->AddComponent<Transform>();
				subtitleTransform->SetLocalPosition({ 219.0f, -90.0f, 0.0f });
				subtitleTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
				subtitleTransform->SetLocalScale({ 150.0f, 25.0f, 1.0f });

				TextView* subtitleText = subtitleGameObject->AddComponent<TextView>();
				subtitleText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
				subtitleText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
				subtitleText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
				subtitleText->SetText("1st Homework");
				subtitleText->SetFontSize(20.0f);
			}
		}

		{
			GameObject* tankGameObject = Instantiate();
			tankGameObject->SetName("Play Tank Button");

			Transform* tankTransform = tankGameObject->AddComponent<Transform>();
			tankTransform->SetLocalPosition({ 0.0f, 40.0f, 0.0f });
			tankTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
			tankTransform->SetLocalScale({ 304.0f, 44.8f, 1.0f });

			TextView* tankText = tankGameObject->AddComponent<TextView>();
			tankText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
			tankText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
			tankText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
			tankText->SetText("PLAY WITH TANK");
			tankText->SetFontSize(32.0f);
			tankText->SetHorizontalAlignment(TextHorizontalAlignment::CENTER);

			ButtonUI* tankButton = tankGameObject->AddComponent<ButtonUI>();
#ifdef _DEBUG
			tankButton->SetDebugMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/DebugUI.tukmat"));
#endif
			tankButton->OnEnter() += [tankText]()
			{
				tankText->SetColor(ColorRGBA<f32>::GetYellow());
			};
			tankButton->OnExit() += [tankText]()
			{
				tankText->SetColor(ColorRGBA<f32>::GetWhite());
			};
			tankButton->OnClick() += []()
			{
				SceneManager::LoadScene("Stage Tank");
			};
		}

		{
			GameObject* jetGameObject = Instantiate();
			jetGameObject->SetName("Play Jet Button");

			Transform* jetTransform = jetGameObject->AddComponent<Transform>();
			jetTransform->SetLocalPosition({ 0.0f, 120.0f, 0.0f });
			jetTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
			jetTransform->SetLocalScale({ 304.0f, 44.8f, 1.0f });

			TextView* jetText = jetGameObject->AddComponent<TextView>();
			jetText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
			jetText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
			jetText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
			jetText->SetText("PLAY WITH JET");
			jetText->SetFontSize(32.0f);
			jetText->SetHorizontalAlignment(TextHorizontalAlignment::CENTER);

			ButtonUI* jetButton = jetGameObject->AddComponent<ButtonUI>();
#ifdef _DEBUG
			jetButton->SetDebugMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/DebugUI.tukmat"));
#endif
			jetButton->OnEnter() += [jetText]()
			{
				jetText->SetColor(ColorRGBA<f32>::GetYellow());
			};
			jetButton->OnExit() += [jetText]()
			{
				jetText->SetColor(ColorRGBA<f32>::GetWhite());
			};
			jetButton->OnClick() += []()
			{
				SceneManager::LoadScene("Stage Jet");
			};
		}

		{
			GameObject* exitGameObject = Instantiate();
			exitGameObject->SetName("Exit Button");

			Transform* exitTransform = exitGameObject->AddComponent<Transform>();
			exitTransform->SetLocalPosition({ 0.0f, 200.0f, 0.0f });
			exitTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
			exitTransform->SetLocalScale({ 73.6f, 44.8f, 1.0f });

			TextView* exitText = exitGameObject->AddComponent<TextView>();
			exitText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
			exitText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
			exitText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
			exitText->SetText("EXIT");
			exitText->SetFontSize(32.0f);

			ButtonUI* exitButton = exitGameObject->AddComponent<ButtonUI>();
#ifdef _DEBUG
			exitButton->SetDebugMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/DebugUI.tukmat"));
#endif
			exitButton->OnEnter() += [exitText]()
			{
				exitText->SetColor(ColorRGBA<f32>::GetYellow());
			};
			exitButton->OnExit() += [exitText]()
			{
				exitText->SetColor(ColorRGBA<f32>::GetWhite());
			};
			exitButton->OnClick() += []()
			{
				App::GetInstance().GetSubsystem<SceneSubsystem>()->ExitScene();
			};
		}

		{
			GameObject* studentInfoGameObject = Instantiate();
			studentInfoGameObject->SetName("Student Info");

			Transform* studentInfoTransform = studentInfoGameObject->AddComponent<Transform>();
			studentInfoTransform->SetLocalPosition({ -190.0f, 275.0f, 0.0f });
			studentInfoTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
			studentInfoTransform->SetLocalScale({ 400.0f, 30.0f, 1.0f });

			TextView* studentInfoText = studentInfoGameObject->AddComponent<TextView>();
			studentInfoText->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
			studentInfoText->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
			studentInfoText->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
			studentInfoText->SetText("2023180030 LEE HYEOUN BEOM");
			studentInfoText->SetFontSize(20.0f);
		}
	}
}
