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
	GameObject* owner{ GetOwner() };
	if (owner == nullptr) return;

	Transform* transform{ owner->GetComponent<Transform>() };
	if (transform == nullptr) return;

	CubeCollider* collider{ owner->GetComponent<CubeCollider>() };
	if (collider == nullptr) return;

	Scene* scene{ owner->GetScene() };
	if (scene == nullptr) return;

	fireTimer += deltaTime_;

	// 1. Handle Rotation (Local Y-axis)
	Quaternion rotation{ transform->GetLocalRotation() };
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::A))
	{
		rotation = rotation * Quaternion::Euler(0.0f, -rotationSpeed * deltaTime_, 0.0f);
	}
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::D))
	{
		rotation = rotation * Quaternion::Euler(0.0f, rotationSpeed * deltaTime_, 0.0f);
	}
	transform->SetLocalRotation(rotation);

	// 2. Fire projectile
	if (InputSystem::GetInstance().IsButtonPressed(ButtonCode::Left) && fireTimer >= fireCooldown)
	{
		fireTimer = 0.0f;

		Mesh* projectileMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Sphere.bin") };
		Material* projectileMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/PlayerProjectile.bin") };
		Mesh* defaultMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Cube.bin") };
		Material* defaultMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"DefaultMaterial") };

		const Vector3D forward{ rotation * Vector3D::GetForward() };
		const Vector3D muzzlePos{ transform->GetWorldPosition() + forward * 1.2f + Vector3D(0.0f, 0.7f, 0.0f) };

		GameObject* projectile{ scene->Instantiate(muzzlePos, Quaternion::LookRotation(forward, Vector3D::GetUp())) };
		projectile->SetName(L"PlayerProjectile");
		projectile->SetTag(L"PlayerProjectile");

		MeshRenderer* projectileRenderer{ projectile->AddComponent<MeshRenderer>() };
		projectileRenderer->SetMesh(projectileMesh != nullptr ? projectileMesh : defaultMesh);
		projectileRenderer->SetMaterial(projectileMaterial != nullptr ? projectileMaterial : defaultMaterial);

		if (projectileMesh != nullptr)
		{
			const Vector3D boundsMin{ projectileMesh->GetBoundsMin() };
			const Vector3D boundsMax{ projectileMesh->GetBoundsMax() };
			CubeCollider* projectileCollider{ projectile->AddComponent<CubeCollider>() };
			projectileCollider->SetCenter((boundsMin + boundsMax) * 0.5f);
			projectileCollider->SetSize((boundsMax - boundsMin) * 0.3f);
			projectileCollider->SetStatic(false);
			projectileCollider->UpdateVolume();
		}

		PlayerProjectile* projectileLogic{ projectile->AddComponent<PlayerProjectile>() };
		projectileLogic->SetDirection(forward);
		projectileLogic->SetSpeed(projectileSpeed);
		projectileLogic->SetLifeTime(projectileLifetime);
	}

	// 3. Calculate Move Delta
	Vector3D moveDelta{ Vector3D::GetZero() };
	Vector3D forward{ rotation * Vector3D::GetForward() };

	if (InputSystem::GetInstance().IsKeyDown(KeyCode::W))
	{
		moveDelta += forward * (moveSpeed * deltaTime_);
	}
	if (InputSystem::GetInstance().IsKeyDown(KeyCode::S))
	{
		moveDelta -= forward * (moveSpeed * deltaTime_);
	}

	// 4. Gravity & Jump
	verticalVelocity += gravity * deltaTime_;
	if (InputSystem::GetInstance().IsKeyPressed(KeyCode::Space) && isGrounded)
	{
		verticalVelocity = jumpSpeed;
		isGrounded = false;
	}
	moveDelta.y = verticalVelocity * deltaTime_;

	// 5. Axis-Separated Move & Revert (with Narrow-Test for smooth sliding)
	const Vector3D originalSize{ collider->GetSize() };
	const float skinWidth{ 0.02f };
	Vector3D currentPos{ transform->GetWorldPosition() };

	// --- X Axis ---
	if (std::abs(moveDelta.x) > Mathf::Epsilon)
	{
		collider->SetSize(Vector3D{ originalSize.x, originalSize.y - skinWidth, originalSize.z - skinWidth });
		transform->SetWorldPosition(Vector3D{ currentPos.x + moveDelta.x, currentPos.y, currentPos.z });
		collider->UpdateVolume();
		if (PhysicsSystem::GetInstance().IsCollidingWithStatic(collider))
		{
			transform->SetWorldPosition(currentPos);
		}
		else
		{
			currentPos.x += moveDelta.x;
		}
	}

	// --- Y Axis (swept in small steps to prevent tunneling through thin floors) ---
	if (std::abs(moveDelta.y) > Mathf::Epsilon)
	{
		collider->SetSize(Vector3D{ originalSize.x - skinWidth, originalSize.y, originalSize.z - skinWidth });

		float remainingY{ moveDelta.y };
		constexpr float maxStepY{ 0.1f };

		while (std::abs(remainingY) > Mathf::Epsilon)
		{
			const float stepY{ std::clamp(remainingY, -maxStepY, maxStepY) };
			transform->SetWorldPosition(Vector3D{ currentPos.x, currentPos.y + stepY, currentPos.z });
			collider->UpdateVolume();

			if (PhysicsSystem::GetInstance().IsCollidingWithStatic(collider))
			{
				if (stepY <= 0.0f)
				{
					isGrounded = true;
				}
				verticalVelocity = 0.0f;
				transform->SetWorldPosition(currentPos);
				collider->UpdateVolume();
				break;
			}

			currentPos.y += stepY;
			remainingY -= stepY;
			isGrounded = false;
		}
	}
	else
	{
		// Probe ground when vertical delta is near zero to keep grounded state stable.
		collider->SetSize(Vector3D{ originalSize.x - skinWidth, originalSize.y, originalSize.z - skinWidth });
		transform->SetWorldPosition(Vector3D{ currentPos.x, currentPos.y - 0.02f, currentPos.z });
		collider->UpdateVolume();
		const bool onGround{ PhysicsSystem::GetInstance().IsCollidingWithStatic(collider) };
		transform->SetWorldPosition(currentPos);
		collider->UpdateVolume();
		isGrounded = onGround;
		if (onGround && verticalVelocity < 0.0f)
		{
			verticalVelocity = 0.0f;
		}
	}

	// --- Z Axis ---
	if (std::abs(moveDelta.z) > Mathf::Epsilon)
	{
		collider->SetSize(Vector3D{ originalSize.x - skinWidth, originalSize.y - skinWidth, originalSize.z });
		transform->SetWorldPosition(Vector3D{ currentPos.x, currentPos.y, currentPos.z + moveDelta.z });
		collider->UpdateVolume();
		if (PhysicsSystem::GetInstance().IsCollidingWithStatic(collider))
		{
			transform->SetWorldPosition(currentPos);
		}
		else
		{
			currentPos.z += moveDelta.z;
		}
	}

	collider->SetSize(originalSize);
	collider->UpdateVolume();
}
