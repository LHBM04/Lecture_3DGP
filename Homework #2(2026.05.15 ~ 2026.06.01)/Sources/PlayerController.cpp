#include "Precompiled.h"
#include "PlayerController.h"

#include "Collider.h"
#include "CubeCollider.h"
#include "GameObject.h"
#include "InputSystem.h"
#include "MathF.h"
#include "PhysicsSystem.h"
#include "Quaternion.h"
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

	// 2. Calculate Move Delta
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

	if (moveDelta.IsZero()) return;

	// 3. Axis-Separated Move & Revert (with Narrow-Test for smooth sliding)
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

	// --- Y Axis ---
	if (std::abs(moveDelta.y) > Mathf::Epsilon)
	{
		collider->SetSize(Vector3D{ originalSize.x - skinWidth, originalSize.y, originalSize.z - skinWidth });
		transform->SetWorldPosition(Vector3D{ currentPos.x, currentPos.y + moveDelta.y, currentPos.z });
		collider->UpdateVolume();
		if (PhysicsSystem::GetInstance().IsCollidingWithStatic(collider))
		{
			transform->SetWorldPosition(currentPos);
		}
		else
		{
			currentPos.y += moveDelta.y;
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
