#include "Precompiled.h"

#include "EnemyController.h"

#include "Collider.h"
#include "CubeCollider.h"
#include "ExplodeParticle.h"
#include "GameObject.h"
#include "Material.h"
#include "MathF.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "PhysicsSystem.h"
#include "Quaternion.h"
#include "ResourceSystem.h"
#include "Scene.h"
#include "Transform.h"
#include "Vector3D.h"

void EnemyController::OnAwake()
{
}

void EnemyController::OnStart()
{
}

void EnemyController::OnUpdate(float deltaTime_)
{
	GameObject* const owner{ GetOwner() };
	Transform* const transform{ owner->GetComponent<Transform>() };
	GameObject* const player{ owner->GetScene()->FindObjectWithTag(L"Player") };

	if (player == nullptr)
	{
		return;
	}

	const Vector3D playerPos{ player->GetComponent<Transform>()->GetWorldPosition() };
	const Vector3D currentPos{ transform->GetWorldPosition() };
	const float distanceToPlayer{ Vector3D::Distance(currentPos, playerPos) };

	if (distanceToPlayer <= detectionRange)
	{
		hasDetectedPlayer = true;
	}

	if (hasDetectedPlayer)
	{
		const Vector3D moveDir{ Vector3D::Normalize(playerPos - currentPos) };
		const Quaternion targetRotation{ Quaternion::LookRotation(moveDir, Vector3D::GetUp()) };
		transform->SetWorldRotation(Quaternion::Slerp(transform->GetWorldRotation(), targetRotation, rotationSpeed * deltaTime_));

		Vector3D moveDelta{ moveDir * (moveSpeed * deltaTime_) };
		moveDelta.y = 0.0f;

		Vector3D currentPos{ transform->GetWorldPosition() };
		const float maxStepHeight{ 0.5f };

		// X-axis move + step-up check
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

		// Z-axis move + step-up check
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
		}
	}

	// Gravity
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

void EnemyController::OnCollisionEnter(Collider* other_)
{
	const GameObject* const otherObject{ other_->GetOwner() };
	if (otherObject->GetTag() == L"PlayerProjectile")
	{
		SpawnExplosionParticles(GetOwner()->GetComponent<Transform>()->GetWorldPosition());
		GetOwner()->GetScene()->Destroy(GetOwner());
	}
}

bool EnemyController::IsColliding(bool ignoreFloor_)
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

void EnemyController::SpawnExplosionParticles(const Vector3D& origin_)
{
	Scene* const scene{ GetOwner()->GetScene() };
	Mesh* const particleMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Sphere.bin") };
	Material* const particleMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/Particle.bin") };

	constexpr int ParticleCount{ 10 };
	for (int i{ 0 }; i < ParticleCount; ++i)
	{
		GameObject* const particle{ scene->Instantiate(origin_) };
		particle->SetName(L"ExplodeParticle");
		
		const Vector3D randomDir{ 
			(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f,
			(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f,
			(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f
		};

		ExplodeParticle* const logic{ particle->AddComponent<ExplodeParticle>() };
		logic->SetVelocity(Vector3D::Normalize(randomDir) * 5.0f);
		
		MeshRenderer* const renderer{ particle->AddComponent<MeshRenderer>() };
		renderer->SetMesh(particleMesh);
		renderer->SetMaterial(particleMaterial);
		
		particle->GetComponent<Transform>()->SetLocalScale(Vector3D{ 0.2f, 0.2f, 0.2f });
	}
}
