#include "Precompiled.h"
#include "PlayerController.h"

#include "Collider.h"
#include "CubeCollider.h"
#include "GameObject.h"
#include "InputSystem.h"
#include "Material.h"
#include "MathF.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "PhysicsSystem.h"
#include "PlayerProjectile.h"
#include "Quaternion.h"
#include "ResourceSystem.h"
#include "Scene.h"
#include "SphereCollider.h"
#include "TimeSystem.h"
#include "Transform.h"
#include "Vector3D.h"

void PlayerController::OnStart()
{
	GameObject* const owner{ GetOwner() };
	if (owner == nullptr) return;

	Transform* const rootTransform{ owner->GetComponent<Transform>() };
	if (rootTransform == nullptr) return;

	// 'skin' 노드(기수 부분)를 찾아 발사 지점으로 설정합니다.
	auto findNode = [&](this auto& self, Transform* current_, std::wstring_view name_) -> Transform*
	{
		if (current_->GetOwner() != nullptr && current_->GetOwner()->GetName() == name_)
		{
			return current_;
		}

		for (Transform* child : current_->GetChildren())
		{
			if (Transform* found = self(child, name_)) return found;
		}
		return nullptr;
	};

	headTransform = findNode(rootTransform, L"skin");
}

void PlayerController::OnUpdate()
{
	GameObject* const owner{ GetOwner() };
	if (owner == nullptr)
	{
		return;
	}

	Transform* const transform{ owner->GetComponent<Transform>() };
	if (transform == nullptr)
	{
		return;
	}

	Quaternion rotation{ transform->GetLocalRotation() };
	float yawInput{ 0.0f };
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::A))
	{
		yawInput -= 1.0f;
	}
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::D))
	{
		yawInput += 1.0f;
	}

	rotation = rotation * Quaternion::Euler(0.0f, yawInput * rotationSpeed * TimeSystem::GetInstance().GetDeltaTime(), 0.0f);
	transform->SetLocalRotation(rotation);

	Vector3D moveDelta{ Vector3D::GetZero() };
	const float deltaTime{ TimeSystem::GetInstance().GetDeltaTime() };
	const Vector3D forward{ rotation * Vector3D::GetForward() };
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::W))
	{
		moveDelta += forward * (moveSpeed * deltaTime);
	}
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::S))
	{
		moveDelta -= forward * (moveSpeed * deltaTime);
	}
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::LeftShift))
	{
		moveDelta.y += altitudeSpeed * deltaTime;
	}
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::LeftControl))
	{
		moveDelta.y -= altitudeSpeed * deltaTime;
	}

	Vector3D currentPos{ transform->GetWorldPosition() };
	const float maxStepHeight{ 0.5f };

	if (std::abs(moveDelta.x) > Mathf::Epsilon)
	{
		transform->SetWorldPosition(Vector3D{ currentPos.x + moveDelta.x, currentPos.y, currentPos.z });

		if (IsColliding(true))
		{
			bool climbed{ false };
			float testY{ currentPos.y };
			while (testY < currentPos.y + maxStepHeight)
			{
				testY += 0.05f;
				transform->SetWorldPosition(Vector3D{ currentPos.x + moveDelta.x, testY, currentPos.z });
				if (!IsColliding(true))
				{
					climbed = true;
					break;
				}
			}

			if (!climbed)
			{
				transform->SetWorldPosition(currentPos);
			}
		}
		currentPos = transform->GetWorldPosition();
	}

	if (std::abs(moveDelta.z) > Mathf::Epsilon)
	{
		transform->SetWorldPosition(Vector3D{ currentPos.x, currentPos.y, currentPos.z + moveDelta.z });

		if (IsColliding(true))
		{
			bool climbed{ false };
			float testY{ currentPos.y };
			while (testY < currentPos.y + maxStepHeight)
			{
				testY += 0.05f;
				transform->SetWorldPosition(Vector3D{ currentPos.x, testY, currentPos.z + moveDelta.z });
				if (!IsColliding(true))
				{
					climbed = true;
					break;
				}
			}

			if (!climbed)
			{
				transform->SetWorldPosition(currentPos);
			}
		}
		currentPos = transform->GetWorldPosition();
	}

	if (std::abs(moveDelta.y) > Mathf::Epsilon)
	{
		transform->SetWorldPosition(Vector3D{ currentPos.x, currentPos.y + moveDelta.y, currentPos.z });
		currentPos = transform->GetWorldPosition();
	}

	fireTimer += deltaTime;
	if (InputSystem::GetInstance().IsButtonDown(ButtonCode::Left) && fireTimer >= fireCooldown)
	{
		Scene* const scene{ owner->GetScene() };
		if (scene != nullptr)
		{
			const Vector3D projectileDirection{ (rotation * Vector3D::GetForward()).GetNormalized() };
			
			// headTransform(기수 부분)이 있으면 그 위치를 기준으로 발사합니다.
			const Vector3D baseSpawnPos{ headTransform != nullptr ? headTransform->GetWorldPosition() : transform->GetWorldPosition() };
			const Vector3D spawnPosition{ baseSpawnPos + projectileDirection * 5.0f };

			GameObject* const projectile{ scene->Instantiate(spawnPosition, rotation) };
			projectile->SetName(L"PlayerProjectile");
			projectile->SetTag(L"PlayerProjectile");

			Mesh* const mesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/PlayerProjectile.bin") };
			Material* const material{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/PlayerProjectile.bin") };

			MeshRenderer* const meshRenderer{ projectile->AddComponent<MeshRenderer>() };
			meshRenderer->SetMesh(mesh);
			meshRenderer->SetMaterial(material);

			Transform* const projectileTransform{ projectile->GetComponent<Transform>() };
			if (projectileTransform != nullptr)
			{
				projectileTransform->SetLocalScale(Vector3D{ 4.0f, 4.0f, 4.0f });
			}

			PlayerProjectile* const projectileLogic{ projectile->AddComponent<PlayerProjectile>() };
			projectileLogic->SetDirection(projectileDirection);
			projectileLogic->SetSpeed(projectileSpeed);
			projectileLogic->SetLifeTime(projectileLifetime);

			SphereCollider* const projectileCollider{ projectile->AddComponent<SphereCollider>() };
			if (projectileCollider != nullptr)
			{
				projectileCollider->SetRadius(0.2f);
				projectileCollider->SetStatic(false);
				projectileCollider->UpdateVolume();
			}
		}
		fireTimer = 0.0f;
	}

}

bool PlayerController::IsColliding(bool ignoreFloor_)
{
	GameObject* const owner{ GetOwner() };
	CubeCollider* const myCollider{ owner->GetComponent<CubeCollider>() };
	if (myCollider == nullptr)
	{
		return false;
	}

	myCollider->UpdateVolume();

	const std::vector<Collider*> nearbyColliders{ PhysicsSystem::GetInstance().GetNearbyStaticColliders(myCollider) };
	for (Collider* const otherCol : nearbyColliders)
	{
		GameObject* const go{ otherCol->GetOwner() };
		if (go == nullptr || go == owner)
		{
			continue;
		}

		if (myCollider->IsIntersects(otherCol))
		{
			if (ignoreFloor_ && go->GetName().find(L"Floor") != std::wstring::npos)
			{
				continue;
			}
			return true;
		}
	}
	return false;
}

void PlayerController::OnCollisionEnter(Collider* other_)
{
	if (other_ == nullptr || other_->GetOwner() == nullptr || gameOverTriggered)
	{
		return;
	}

	const std::wstring& otherTag{ other_->GetOwner()->GetTag() };
	const std::wstring& otherName{ other_->GetOwner()->GetName() };

	// 디버깅을 위한 로그 추가
	Logger::Info(L"충돌 발생! 대상 이름: {}, 태그: {}", otherName, otherTag);

	if (otherTag == L"Enemy" || otherTag == L"EnemyProjectile")
	{
		gameOverTriggered = true;
		std::wstring errorMsg = std::format(L"Game Over! 원인: {} ({})", otherName, otherTag);
		MessageBoxW(nullptr, errorMsg.c_str(), L"Game Over", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}
}
