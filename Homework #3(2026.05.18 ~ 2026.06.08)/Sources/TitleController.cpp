#include "Precompiled.h"
#include "TitleController.h"

#include <cmath>
#include <numbers>

#include "AnimationClip.h"
#include "Animator.h"
#include "Camera.h"
#include "CubeCollider.h"
#include "ExplodeParticle.h"
#include "GameObject.h"
#include "InputSystem.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "PhysicsSystem.h"
#include "ResourceSystem.h"
#include "Scene.h"
#include "SceneSystem.h"
#include "TimeSystem.h"
#include "Transform.h"
#include "Vector3D.h"

void TitleController::SetTitleLogoObject(GameObject* titleLogoObject_) noexcept
{
	titleLogoObject = titleLogoObject_;
}

void TitleController::SetNameObject(GameObject* nameObject_) noexcept
{
	nameObject = nameObject_;
}

void TitleController::SetTutorialButtonObject(GameObject* tutorialButtonObject_) noexcept
{
	tutorialButtonObject = tutorialButtonObject_;
}

void TitleController::SetLevel1ButtonObject(GameObject* level1ButtonObject_) noexcept
{
	level1ButtonObject = level1ButtonObject_;
}

void TitleController::SetLevel2ButtonObject(GameObject* level2ButtonObject_) noexcept
{
	level2ButtonObject = level2ButtonObject_;
}

void TitleController::SetLevel3ButtonObject(GameObject* level3ButtonObject_) noexcept
{
	level3ButtonObject = level3ButtonObject_;
}

void TitleController::SetStartButtonObject(GameObject* startButtonObject_) noexcept
{
	startButtonObject = startButtonObject_;
}

void TitleController::SetEndButtonObject(GameObject* endButtonObject_) noexcept
{
	endButtonObject = endButtonObject_;
}

void TitleController::OnStart()
{
	HideMenuButtons();
}

void TitleController::OnUpdate()
{
	UpdateTitleLogoRotation();

	if (!InputSystem::GetInstance().IsButtonPressed(ButtonCode::Left))
	{
		return;
	}

	GameObject* const owner{ GetOwner() };
	Scene* const scene{ owner != nullptr ? owner->GetScene() : nullptr };
	if (scene == nullptr)
	{
		return;
	}

	const std::span<Camera* const> cameras{ scene->GetCameras() };
	if (cameras.empty() || cameras.front() == nullptr)
	{
		return;
	}

	Vector3D rayOrigin{ Vector3D::GetZero() };
	Vector3D rayDirection{ Vector3D::GetForward() };
	cameras.front()->ScreenPointToRay(InputSystem::GetInstance().GetMousePosition(), rayOrigin, rayDirection);

	PhysicsSystem::RaycastHit hitInfo{};
	if (!PhysicsSystem::GetInstance().Raycast(rayOrigin, rayDirection, hitInfo))
	{
		return;
	}

	GameObject* const clickedObject{ hitInfo.gameObject };
	if (clickedObject == nullptr)
	{
		return;
	}

	if (!isMenuVisible)
	{
		if (clickedObject == titleLogoObject)
		{
			ExplodeTitleLogo();
			nameObject->SetActive(false);
			RevealMenuButtons();
		}

		return;
	}

	if (clickedObject == tutorialButtonObject)
	{
		SelectLevel(L"Level0", tutorialButtonObject);
		return;
	}

	if (clickedObject == level1ButtonObject)
	{
		SelectLevel(L"Level1", level1ButtonObject);
		return;
	}

	if (clickedObject == level2ButtonObject)
	{
		SelectLevel(L"Level2", level2ButtonObject);
		return;
	}

	if (clickedObject == level3ButtonObject)
	{
		SelectLevel(L"Level3", level3ButtonObject);
		return;
	}

	if (clickedObject == startButtonObject)
	{
		if (selectedSceneName.empty())
		{
			Logger::Info(L"Select a stage first.");
			return;
		}

		Logger::Info(L"Loading scene: {}", selectedSceneName);
		SceneSystem::GetInstance().LoadScene(selectedSceneName);
		return;
	}

	if (clickedObject == endButtonObject)
	{
		Logger::Info(L"Exiting game.");
		SceneSystem::GetInstance().RequestQuit();
	}
}

void TitleController::UpdateTitleLogoRotation()
{
	if (titleLogoObject == nullptr || titleLogoObject->IsDestroyed())
	{
		return;
	}

	Transform* const logoTransform{ titleLogoObject->GetComponent<Transform>() };
	if (logoTransform == nullptr)
	{
		return;
	}

	constexpr float rotationSpeed{ 35.0f };
	const Quaternion rotationDelta{ Quaternion::Euler(0.0f, rotationSpeed * TimeSystem::GetInstance().GetDeltaTime(), 0.0f) };
	logoTransform->SetLocalRotation(logoTransform->GetLocalRotation() * rotationDelta);
}

void TitleController::RevealMenuButtons()
{
	isMenuVisible = true;

	if (tutorialButtonObject != nullptr) tutorialButtonObject->SetActive(true);
	if (level1ButtonObject != nullptr) level1ButtonObject->SetActive(true);
	if (level2ButtonObject != nullptr) level2ButtonObject->SetActive(true);
	if (level3ButtonObject != nullptr) level3ButtonObject->SetActive(true);
	if (startButtonObject != nullptr) startButtonObject->SetActive(true);
	if (endButtonObject != nullptr) endButtonObject->SetActive(true);
}

void TitleController::HideMenuButtons()
{
	if (tutorialButtonObject != nullptr) tutorialButtonObject->SetActive(false);
	if (level1ButtonObject != nullptr) level1ButtonObject->SetActive(false);
	if (level2ButtonObject != nullptr) level2ButtonObject->SetActive(false);
	if (level3ButtonObject != nullptr) level3ButtonObject->SetActive(false);
	if (startButtonObject != nullptr) startButtonObject->SetActive(false);
	if (endButtonObject != nullptr) endButtonObject->SetActive(false);
}

void TitleController::ExplodeTitleLogo()
{
	if (titleLogoObject == nullptr)
	{
		return;
	}

	Scene* const scene{ titleLogoObject->GetScene() };
	Transform* const logoTransform{ titleLogoObject->GetComponent<Transform>() };
	if (scene == nullptr || logoTransform == nullptr)
	{
		return;
	}

	ResourceSystem& resourceSystem{ ResourceSystem::GetInstance() };
	Mesh* const particleMesh{ resourceSystem.GetResource<Mesh>(L"Resources/Meshes/Particle.bin") };
	Material* const particleMaterial{ resourceSystem.GetResource<Material>(L"Resources/Materials/ExplodeParticle.bin") };
	if (particleMesh == nullptr || particleMaterial == nullptr)
	{
		Logger::Warning(L"Title logo explosion particle resource not found.");
		scene->Destroy(titleLogoObject);
		titleLogoObject = nullptr;
		return;
	}

	Vector3D origin{ logoTransform->GetWorldPosition() };
	if (CubeCollider* const logoCollider{ titleLogoObject->GetComponent<CubeCollider>() }; logoCollider != nullptr)
	{
		logoCollider->UpdateVolume();
		const DirectX::BoundingOrientedBox& logoVolume{ logoCollider->GetVolume() };
		origin = Vector3D(logoVolume.Center.x, logoVolume.Center.y, logoVolume.Center.z);
	}

	std::uniform_real_distribution<float> speedDistribution(18.0f, 32.0f);
	std::uniform_real_distribution<float> upwardDistribution(8.0f, 18.0f);
	std::uniform_real_distribution<float> angleJitterDistribution(-0.25f, 0.25f);

	constexpr int particleCount{ 16 };
	for (int i{ 0 }; i < particleCount; ++i)
	{
		const float angle{ (std::numbers::pi_v<float> * 2.0f * static_cast<float>(i) / static_cast<float>(particleCount)) + angleJitterDistribution(randomEngine) };
		const float speed{ speedDistribution(randomEngine) };
		const Vector3D velocity
		{
			std::cos(angle) * speed,
			upwardDistribution(randomEngine),
			std::sin(angle) * speed
		};

		GameObject* const particleObject{ scene->Instantiate(origin) };
		if (particleObject == nullptr)
		{
			continue;
		}

		particleObject->SetName(L"TitleLogoParticle");

		if (Transform* const particleTransform{ particleObject->GetComponent<Transform>() }; particleTransform != nullptr)
		{
			particleTransform->SetLocalScale(Vector3D(2.5f, 2.5f, 2.5f));
			particleTransform->SetLocalRotation(Quaternion::Euler(0.0f, 180.0f, 0.0f));
		}

		MeshRenderer* const meshRenderer{ particleObject->AddComponent<MeshRenderer>() };
		meshRenderer->SetMesh(particleMesh);
		meshRenderer->SetMaterial(particleMaterial);

		ExplodeParticle* const particle{ particleObject->AddComponent<ExplodeParticle>() };
		particle->SetVelocity(velocity);
		particle->SetLifeTime(0.8f);
		particle->SetGravity(-22.0f);
	}

	scene->Destroy(titleLogoObject);
	titleLogoObject = nullptr;
}

void TitleController::SelectLevel(std::wstring_view sceneName_, GameObject* buttonObject_)
{
	ResourceSystem& resourceSystem{ ResourceSystem::GetInstance() };
	AnimationClip* const normalAnimation{ resourceSystem.GetResource<AnimationClip>(L"Resources/Animations/Normal.bin") };
	AnimationClip* const selectedAnimation{ resourceSystem.GetResource<AnimationClip>(L"Resources/Animations/Selected.bin") };

	auto applyAnimation = [&](GameObject* object_, bool isSelected_) -> void
	{
		if (object_ == nullptr)
		{
			return;
		}

		if (!isSelected_)
		{
			if (Transform* const transform{ object_->GetComponent<Transform>() }; transform != nullptr)
			{
				transform->SetLocalRotation(Quaternion::GetIdentity());
			}
		}

		Animator* const animator{ object_->GetComponent<Animator>() };
		if (animator == nullptr)
		{
			return;
		}

		animator->Play(isSelected_ ? selectedAnimation : normalAnimation, true);
	};

	applyAnimation(tutorialButtonObject, sceneName_ == L"Level0");
	applyAnimation(level1ButtonObject, sceneName_ == L"Level1");
	applyAnimation(level2ButtonObject, sceneName_ == L"Level2");
	applyAnimation(level3ButtonObject, sceneName_ == L"Level3");

	selectedSceneName = sceneName_;
	Logger::Info(L"Selected stage: {}", selectedSceneName);
	(void)buttonObject_;
}
