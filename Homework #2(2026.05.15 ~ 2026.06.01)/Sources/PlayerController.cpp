#include "Precompiled.h"
#include "PlayerController.h"

#include <windows.h>

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
#include "Transform.h"
#include "Vector3D.h"

void PlayerController::OnAwake()
{
}

void PlayerController::OnStart()
{
}

void PlayerController::OnUpdate(float deltaTime_)
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

	const float yawSpeed{ 120.0f };
	rotation = rotation * Quaternion::Euler(0.0f, yawInput * yawSpeed * deltaTime_, 0.0f);
	transform->SetLocalRotation(rotation);

	Vector3D moveDelta{ Vector3D::GetZero() };
	const Vector3D forward{ rotation * Vector3D::GetForward() };
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::W))
	{
		moveDelta += forward * (moveSpeed * deltaTime_);
	}
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::S))
	{
		moveDelta -= forward * (moveSpeed * deltaTime_);
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

	fireTimer += deltaTime_;
	if (InputSystem::GetInstance().IsButtonDown(ButtonCode::Left) && fireTimer >= fireCooldown)
	{
		Scene* const scene{ owner->GetScene() };
		if (scene != nullptr)
		{
			const Vector3D projectileDirection{ (rotation * Vector3D::GetForward()).GetNormalized() };
			const Vector3D spawnPosition{ transform->GetWorldPosition() + projectileDirection * 1.0f + Vector3D{ 0.0f, 1.0f, 0.0f } };

			GameObject* const projectile{ scene->Instantiate(spawnPosition, rotation) };
			projectile->SetName(L"PlayerProjectile");
			projectile->SetTag(L"PlayerProjectile");

			Mesh* const mesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Sphere.bin") };
			Material* const material{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/PlayerProjectile.bin") };

			MeshRenderer* const meshRenderer{ projectile->AddComponent<MeshRenderer>() };
			meshRenderer->SetMesh(mesh);
			meshRenderer->SetMaterial(material);

			Transform* const projectileTransform{ projectile->GetComponent<Transform>() };
			if (projectileTransform != nullptr)
			{
				projectileTransform->SetLocalScale(Vector3D{ 0.15f, 0.15f, 0.4f });
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

	if (isGrounded && InputSystem::GetInstance().IsKeyPressed(KeyCode::Space))
	{
		verticalVelocity = jumpSpeed;
		isGrounded = false;
	}

	verticalVelocity += gravity * deltaTime_;
	float remainingY{ verticalVelocity * deltaTime_ };
	const float maxStepY{ 0.1f };
	bool blockedOnY{ false };
	for (int stepIndex{ 0 }; stepIndex < 64 && std::abs(remainingY) > Mathf::Epsilon; ++stepIndex)
	{
		const float stepY{ std::clamp(remainingY, -maxStepY, maxStepY) };
		const Vector3D beforeStepPos{ transform->GetWorldPosition() };
		transform->SetWorldPosition(Vector3D{ beforeStepPos.x, beforeStepPos.y + stepY, beforeStepPos.z });

		if (IsColliding(false))
		{
			transform->SetWorldPosition(beforeStepPos);
			blockedOnY = true;
			if (verticalVelocity < 0.0f)
			{
				isGrounded = true;
			}
			verticalVelocity = 0.0f;
			break;
		}

		remainingY -= stepY;
	}
	if (!blockedOnY)
	{
		isGrounded = false;
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

	if (other_->GetOwner()->GetTag() == L"Enemy")
	{
		gameOverTriggered = true;
		MessageBoxW(nullptr, L"Game Over!", L"Game Over", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}
}

