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

void EnemyController::OnUpdate(float deltaTime_)
{
	GameObject* owner{ GetOwner() };
	if (owner == nullptr)
	{
		return;
	}

	Transform* transform{ owner->GetComponent<Transform>() };
	CubeCollider* myCollider{ owner->GetComponent<CubeCollider>() };
	Scene* scene{ owner->GetScene() };
	if (transform == nullptr || myCollider == nullptr || scene == nullptr)
	{
		return;
	}

	GameObject* player{ scene->FindObjectWithTag(L"Player") };
	if (player == nullptr || !player->IsActive())
	{
		return;
	}

	Transform* playerTransform{ player->GetComponent<Transform>() };
	if (playerTransform == nullptr)
	{
		return;
	}

	const Vector3D toPlayer{ playerTransform->GetWorldPosition() - transform->GetWorldPosition() };
	const float distanceToPlayer{ toPlayer.GetMagnitude() };
	Vector3D moveDelta{ Vector3D::GetZero() };

	if (distanceToPlayer <= detectRange && distanceToPlayer > Mathf::Epsilon)
	{
		Vector3D moveDir{ toPlayer };
		moveDir.y = 0.0f;
		if (!moveDir.IsZero())
		{
			moveDir.Normalize();
			moveDelta = moveDir * (moveSpeed * deltaTime_);

			const Quaternion targetRotation{ Quaternion::LookRotation(moveDir, Vector3D::GetUp()) };
			const Quaternion currentRotation{ transform->GetLocalRotation() };
			const float t{ std::min(1.0f, rotationSpeed * deltaTime_ / 180.0f) };
			transform->SetLocalRotation(Quaternion::Slerp(currentRotation, targetRotation, t));
		}
	}

	Vector3D currentPos{ transform->GetLocalPosition() };

	auto IsColliding = [&](bool ignoreFloor) -> bool
	{
		myCollider->UpdateVolume();
		std::vector<Collider*> nearbyColliders{ PhysicsSystem::GetInstance().GetNearbyStaticColliders(myCollider) };
		for (Collider* otherCol : nearbyColliders)
		{
			GameObject* otherObject{ otherCol->GetOwner() };
			if (otherObject == nullptr || otherObject == owner)
			{
				continue;
			}

			if (!myCollider->IsIntersects(otherCol))
			{
				continue;
			}

			if (ignoreFloor && otherObject->GetName().find(L"Floor") != std::wstring::npos)
			{
				continue;
			}

			return true;
		}
		return false;
	};

	if (std::abs(moveDelta.x) > Mathf::Epsilon)
	{
		transform->SetLocalPosition(Vector3D(currentPos.x + moveDelta.x, currentPos.y, currentPos.z));
		if (IsColliding(true))
		{
			bool climbed{ false };
			float testY{ currentPos.y };
			while (testY < currentPos.y + maxStepHeight)
			{
				testY += 0.05f;
				transform->SetLocalPosition(Vector3D(currentPos.x + moveDelta.x, testY, currentPos.z));
				if (!IsColliding(true))
				{
					climbed = true;
					break;
				}
			}
			if (!climbed)
			{
				transform->SetLocalPosition(currentPos);
			}
		}
		currentPos = transform->GetLocalPosition();
	}

	if (std::abs(moveDelta.z) > Mathf::Epsilon)
	{
		transform->SetLocalPosition(Vector3D(currentPos.x, currentPos.y, currentPos.z + moveDelta.z));
		if (IsColliding(true))
		{
			bool climbed{ false };
			float testY{ currentPos.y };
			while (testY < currentPos.y + maxStepHeight)
			{
				testY += 0.05f;
				transform->SetLocalPosition(Vector3D(currentPos.x, testY, currentPos.z + moveDelta.z));
				if (!IsColliding(true))
				{
					climbed = true;
					break;
				}
			}
			if (!climbed)
			{
				transform->SetLocalPosition(currentPos);
			}
		}
		currentPos = transform->GetLocalPosition();
	}

	verticalVelocity += gravity * deltaTime_;
	const float yDelta{ verticalVelocity * deltaTime_ };
	transform->SetLocalPosition(Vector3D(currentPos.x, currentPos.y + yDelta, currentPos.z));

	if (IsColliding(false))
	{
		if (verticalVelocity < 0.0f)
		{
			isGrounded = true;
		}
		verticalVelocity = 0.0f;
		transform->SetLocalPosition(currentPos);
	}
	else
	{
		isGrounded = false;
	}
}

void EnemyController::OnCollisionEnter(Collider* other_)
{
	if (other_ == nullptr)
	{
		return;
	}

	GameObject* owner{ GetOwner() };
	GameObject* otherObject{ other_->GetOwner() };
	if (owner == nullptr || otherObject == nullptr)
	{
		return;
	}

	auto SpawnExplosionParticles = [&](const Vector3D& origin_)
	{
		Scene* scene{ owner->GetScene() };
		if (scene == nullptr)
		{
			return;
		}

		Mesh* particleMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Cube.bin") };
		if (std::rand() % 2 == 0)
		{
			particleMesh = ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Sphere.bin");
		}
		Material* particleMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/Particle.bin") };

		constexpr int ParticleCount{ 14 };
		for (int i = 0; i < ParticleCount; ++i)
		{
			const float rx{ (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f };
			const float ry{ (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 1.2f + 0.2f };
			const float rz{ (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f };
			Vector3D velocity(rx, ry, rz);
			velocity.Normalize();
			velocity *= 5.0f + (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 3.0f;

			GameObject* particle{ scene->Instantiate(origin_, Quaternion::GetIdentity()) };
			particle->SetName(L"ExplosionParticle");
			particle->SetTag(L"Effect");

			MeshRenderer* renderer{ particle->AddComponent<MeshRenderer>() };
			renderer->SetMesh(particleMesh);
			renderer->SetMaterial(particleMaterial);

			Transform* particleTransform{ particle->GetComponent<Transform>() };
			particleTransform->SetLocalScale(Vector3D(0.08f, 0.08f, 0.08f));

			ExplodeParticle* explodeParticle{ particle->AddComponent<ExplodeParticle>() };
			explodeParticle->SetVelocity(velocity);
			explodeParticle->SetLifeTime(0.55f);
			explodeParticle->SetGravity(-16.0f);
		}
	};

	if (otherObject->GetTag() == L"PlayerProjectile")
	{
		Transform* enemyTransform{ owner->GetComponent<Transform>() };
		if (enemyTransform != nullptr)
		{
			SpawnExplosionParticles(enemyTransform->GetWorldPosition() + Vector3D(0.0f, 0.5f, 0.0f));
		}

		Scene* scene{ owner->GetScene() };
		if (scene != nullptr)
		{
			scene->Destroy(otherObject);
			scene->Destroy(owner);
		}
	}
	else if (otherObject->GetTag() == L"Player")
	{
		Transform* playerTransform{ otherObject->GetComponent<Transform>() };
		if (playerTransform != nullptr)
		{
			SpawnExplosionParticles(playerTransform->GetWorldPosition() + Vector3D(0.0f, 0.5f, 0.0f));
		}

		Scene* scene{ owner->GetScene() };
		if (scene != nullptr)
		{
			scene->Destroy(otherObject);
		}

		::MessageBoxW(nullptr, L"Game Over!", L"Game Over", MB_OK | MB_ICONINFORMATION);
		::PostQuitMessage(0);
	}
}
