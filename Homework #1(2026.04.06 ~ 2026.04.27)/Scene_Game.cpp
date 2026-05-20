#include "Precompiled.hpp"
#include "Scene_Game.hpp"

#include "CameraController.hpp"
#include "CollisionFrustum.hpp"
#include "Input.hpp"
#include "Mesh.hpp"
#include "MeshRenderer.hpp"
#include "ObstacleSpawner.hpp"
#include "Object.hpp"
#include "OrthographicsCamera.hpp"
#include "PerspectiveCamera.hpp"
#include "PlayerMovement.hpp"
#include "Renderer.hpp"
#include "TextView.hpp"
#include "Transform.hpp"
#include "Window.hpp"

std::size_t Scene_Game::GetPlayerScore()
{
	return playerScore;
}

void Scene_Game::SetPlayerScore(std::size_t score_)
{
	playerScore = score_;
}

void Scene_Game::AddPlayerScore(std::size_t score_)
{
	playerScore += score_;
}

void Scene_Game::OnLoad()
{
	SetPlayerScore(0);

	Object* playerObject = AddObject();
	playerObject->SetName("Player");
	playerObject->SetTag("Player");

	[[maybe_unused]] PlayerMovement* playerMovement = playerObject->AddComponent<PlayerMovement>();
	[[maybe_unused]] ObstacleSpawner* obstacleSpawner = playerObject->AddComponent<ObstacleSpawner>();

	MeshRenderer* meshRenderer = playerObject->AddComponent<MeshRenderer>();
	meshRenderer->SetColor(ColorRGBA::GetWhite());
	meshRenderer->SetMesh(Mesh::LoadFromFile("./Assets/Airplane.obj"));

	cameraObject = AddObject();
	cameraObject->SetName("Main Camera");
	cameraObject->SetTag("MainCamera");

	Camera* camera = cameraObject->AddComponent<PerspectiveCamera>();
	ApplyCameraSettings(camera);
	
	[[maybe_unused]] CollisionFrustum* cameraFrustum = cameraObject->AddComponent<CollisionFrustum>();

	CameraController* cameraController = cameraObject->AddComponent<CameraController>();
	cameraController->SetTarget(playerObject->GetTransform());
	cameraController->SetOffset(Vector3D(0.0f, 2.0f, -4.5f));

	Object* scoreUIObject = AddObject();
	scoreUIObject->SetName("ScoreText");
	scoreUIObject->SetTag("UI");

	scoreTextView = scoreUIObject->AddComponent<TextView>();
	if (scoreTextView)
	{
		RECT scoreRect
		{
			0,
			16,
			Renderer::GetWidth() - 20,
			64
		};
		scoreTextView->SetRect(scoreRect);
		scoreTextView->SetFontSize(32);
		scoreTextView->SetFormat(DT_TOP | DT_RIGHT | DT_SINGLELINE);
		scoreTextView->SetText(L"Score : 0");
	}
}

void Scene_Game::OnUpdate()
{
	if (scoreTextView)
	{
		scoreTextView->SetText(L"Score : " + std::to_wstring(GetPlayerScore()));
	}

	if (Input::IsKeyPressed('R'))
	{
		ToggleCameraProjection();
	}

	if (Input::IsKeyPressed(VK_ESCAPE))
	{
		// 원래는 이렇게 하면 안 되는데, 어차피 창 닫히면 프로그램도 종료니까,,,
		Window::SetShouldClose(true);
	}
}

void Scene_Game::OnRender()
{
}

void Scene_Game::ToggleCameraProjection()
{
	if (!cameraObject)
	{
		return;
	}

	if (isOrthographicCameraActive)
	{
		cameraObject->RemoveComponent<OrthographicsCamera>();
		Camera* perspectiveCamera = cameraObject->AddComponent<PerspectiveCamera>();
		ApplyCameraSettings(perspectiveCamera);
		isOrthographicCameraActive = false;
		return;
	}

	cameraObject->RemoveComponent<PerspectiveCamera>();
	OrthographicsCamera* orthographicsCamera = cameraObject->AddComponent<OrthographicsCamera>();
	if (orthographicsCamera)
	{
		orthographicsCamera->SetOrthographicSize(8.0f);
		ApplyCameraSettings(orthographicsCamera);
	}
	isOrthographicCameraActive = true;
}

void Scene_Game::ApplyCameraSettings(Camera* camera_) const
{
	if (!camera_)
	{
		return;
	}

	camera_->SetViewport(0.0f, 0.0f, 1280.0f, 720.0f);
	camera_->SetNearClipPlane(0.1f);
	camera_->SetFarClipPlane(200.0f);
}

std::size_t Scene_Game::playerScore = 0;
