#include "Precompiled.hpp"
#include "World_Title.hpp"

#include "Framework/Core.hpp"
#include "Framework/UI.hpp"
#include "Framework/Resources.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Platform/DisplaySubsystem.hpp"

#include "../WorldTransition.hpp"

namespace TUK::Game
{
	World_Title::World_Title(World* stageWorld, WorldTransition* transition) noexcept
		: stageWorld(stageWorld)
		, transition(transition)
	{
		ASSERT_FMT(stageWorld, "전환할 스테이지 월드가 nullptr입니다.");
		ASSERT_FMT(transition, "월드 전환 객체가 nullptr입니다.");
	}

	void World_Title::OnEnter()
	{
		AssetSubsystem* assetSubsystem = App::GetInstance().GetSubsystem<AssetSubsystem>();
		ASSERT_FMT(assetSubsystem, "에셋 서브시스템을 가져오지 못했습니다.");

		// 카메라 세팅
		{
			Entity* cameraEntity = Instantiate();
			cameraEntity->SetName("Camera");
			cameraEntity->SetTag("Main Camera");

			Transform* cameraTransform = cameraEntity->AddComponent<Transform>();
			cameraTransform->SetLocalRotation(Quaternion<f32>{});
			cameraTransform->SetLocalPosition(Vector3D<f32>(0.0f, 0.0f, -10.0f));

			const DisplaySubsystem* displays = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
			ASSERT_FMT(displays, "디스플레이 서브시스템을 가져오지 못했습니다.");

			const Display* display = displays->GetDisplay(0);
			ASSERT_FMT(display, "타이틀 화면을 표시할 디스플레이가 없습니다.");

			const Vector2D<i32> clientSize = display->GetClientSize();
			Camera* camera = cameraEntity->AddComponent<Camera>();
			camera->SetViewport(CD3DX12_VIEWPORT(
				0.0f, 0.0f, static_cast<f32>(clientSize.GetX()), static_cast<f32>(clientSize.GetY())));
			camera->SetScissorRect(CD3DX12_RECT(0, 0, clientSize.GetX(), clientSize.GetY()));
			camera->SetClearFlags(CameraClearFlags::SolidColor);
			camera->SetBackgroundColor(ColorRGBA<f32>::GetBlue());
		}

		// 타이틀 로고 세팅
		{
			Entity* logoEntity = Instantiate();
			logoEntity->SetName("Title Logo");

			Transform* logoTransform = logoEntity->AddComponent<Transform>();
			logoTransform->SetLocalPosition({ 0.0f, -200.0f, 0.0f });
			logoTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
			logoTransform->SetLocalScale({ 1000.0f, 450.0f, 1.0f });

			ImageView* titleImage = logoEntity->AddComponent<ImageView>();
			titleImage->SetImage(assetSubsystem->LoadAsset<TextureAsset>("Resources/Textures/Title.dds"));
		}

		// 시작 버튼 세팅
		{
			const TextureAsset* startTexture = assetSubsystem->LoadAsset<TextureAsset>("Resources/Textures/Start.dds");
			ASSERT_FMT(startTexture, "시작 버튼 텍스처를 로드하지 못했습니다.");

			const TextureAsset* clickTexture = assetSubsystem->LoadAsset<TextureAsset>("Resources/Textures/Quit.dds");
			ASSERT_FMT(clickTexture, "클릭 시 사용할 텍스처를 로드하지 못했습니다.");
		}

		Entity* startEntity = Instantiate();
		startEntity->SetName("Start Button");

		Transform* startTransform = startEntity->AddComponent<Transform>();
		startTransform->SetLocalPosition({ 0.0f, 170.0f, 0.0f });
		startTransform->SetLocalRotation(Quaternion<f32>{0.0f, 0.0f, 0.0f, 1.0f});
		startTransform->SetLocalScale({ 350.0f, 200.0f, 1.0f });

		ImageView* startImage = startEntity->AddComponent<ImageView>();
		startImage->SetImage(assetSubsystem->LoadAsset<TextureAsset>("Resources/Textures/Start.dds"));

		ButtonUI* startButton = startEntity->AddComponent<ButtonUI>();
		startButton->OnEnter() += [startImage, assetSubsystem]()
		{
			startImage->SetImage(assetSubsystem->LoadAsset<TextureAsset>("Resources/Textures/Quit.dds"));
		};
		startButton->OnExit() += [startImage, assetSubsystem]()
		{
			startImage->SetImage(assetSubsystem->LoadAsset<TextureAsset>("Resources/Textures/Start.dds"));
		};
		startButton->OnClick() += [this]()
		{
			if (!transition->Request(stageWorld))
			{
				LOG_ERROR("스테이지 페이드 전환을 시작하지 못했습니다.");
			}
		};
	}

	void World_Title::OnExit()
	{
	}
}
