#include "Precompiled.hpp"
#include "World_Title.hpp"

#include "Framework/Core.hpp"
#include "Framework/UI.hpp"
#include "Framework/Resources.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Platform/DisplaySubsystem.hpp"

#include "../UI/WorldTransition.hpp"

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

		const RenderSubsystem* renderer = Engine::GetInstance().GetSubsystem<RenderSubsystem>();
		ASSERT_FMT(renderer, "렌더 서브시스템을 가져오지 못했습니다.");
		const GPUDevice& device = renderer->GetDevice();
		const ShaderAsset* imageVertex = assetSubsystem->LoadAsset<ShaderAsset>("Resources/Shaders/ImageUI.vert.cso");
		const ShaderAsset* imagePixel = assetSubsystem->LoadAsset<ShaderAsset>("Resources/Shaders/ImageUI.frag.cso");

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
			camera->SetViewport(CD3DX12_VIEWPORT(
				0.0f, 0.0f, static_cast<f32>(clientSize.GetX()), static_cast<f32>(clientSize.GetY())));
			camera->SetScissorRect(CD3DX12_RECT(0, 0, clientSize.GetX(), clientSize.GetY()));
			camera->SetClearColor(ColorRGBA<f32>::GetBlue());
		}

		// 타이틀 로고 세팅
		{
			GameObject* logoGameObject = Instantiate();
			logoGameObject->SetName("Title Logo");

			Transform* logoTransform = logoGameObject->AddComponent<Transform>();
			logoTransform->SetLocalPosition({ 0.0f, -200.0f, 0.0f });
			logoTransform->SetLocalRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
			logoTransform->SetLocalScale({ 1000.0f, 450.0f, 1.0f });

			ImageView* titleImage = logoGameObject->AddComponent<ImageView>();
			titleImage->SetImage(assetSubsystem->LoadAsset<TextureAsset>("Resources/Textures/Title.dds"));
			titleImage->SetMaterial(assetSubsystem->LoadAsset<MaterialAsset>("Resources/Materials/ImageUI.tukmat"));
		}

		// 시작 버튼 세팅
		{
			GameObject* startGameObject = Instantiate();
			startGameObject->SetName("Start Button");

			Transform* startTransform = startGameObject->AddComponent<Transform>();
			startTransform->SetLocalPosition({ 0.0f, 170.0f, 0.0f });
			startTransform->SetLocalRotation({0.0f, 0.0f, 0.0f, 1.0f});
			startTransform->SetLocalScale({ 350.0f, 200.0f, 1.0f });

			ImageView* startImage = startGameObject->AddComponent<ImageView>();
			startImage->SetImage(assetSubsystem->LoadAsset<TextureAsset>("Resources/Textures/Start.dds"));
			startImage->SetMaterial(assetSubsystem->LoadAsset<MaterialAsset>("Resources/Materials/ImageUI.tukmat"));

			ButtonUI* startButton = startGameObject->AddComponent<ButtonUI>();
			startButton->OnEnter() += [startImage, assetSubsystem]() {
				startImage->SetImage(assetSubsystem->LoadAsset<TextureAsset>("Resources/Textures/Quit.dds"));
			};
			startButton->OnExit() += [startImage, assetSubsystem]() {
				startImage->SetImage(assetSubsystem->LoadAsset<TextureAsset>("Resources/Textures/Start.dds"));
			};
			startButton->OnClick() += [this]() {
				if (!transition->Request(stageWorld))
				{
					LOG_ERROR("스테이지 페이드 전환을 시작하지 못했습니다.");
				}
			};
		}
	}

	void World_Title::OnExit()
	{
	}
}
