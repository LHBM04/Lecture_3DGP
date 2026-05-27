#include "Precompiled.hpp"
#include "PlayerMovement.hpp"

#include "Bullet.hpp"
#include "CollisionSphere.hpp"
#include "Input.hpp"
#include "Mesh.hpp"
#include "MeshRenderer.hpp"
#include "Object.hpp"
#include "Quaternion.hpp"
#include "Scene.hpp"
#include "Timer.hpp"
#include "Transform.hpp"
#include "Vector3D.hpp"

float PlayerMovement::GetMoveSpeed() const
{
	return moveSpeed;
}

void PlayerMovement::SetMoveSpeed(float moveSpeed_)
{
	moveSpeed = moveSpeed_;
}

void PlayerMovement::OnAttach()
{

}

void PlayerMovement::OnUpdate()
{
	Transform* transform = GetTransform();
	if (!transform)
	{
		return;
	}

	const float deltaTime = Timer::GetDeltaTime();
	fireTimer = std::max(0.0f, fireTimer - deltaTime);

	Quaternion currentRotation = transform->GetWorldRotation();
	float yawInput = 0.0f;
	if (Input::IsKeyHeld(VK_LEFT))
	{
		yawInput -= 1.0f;
	}
	if (Input::IsKeyHeld(VK_RIGHT))
	{
		yawInput += 1.0f;
	}
	if (std::abs(yawInput) > Mathf::EPSILON)
	{
		const Quaternion deltaRotation = Quaternion::Euler(0.0f, yawInput * rotationSpeed * deltaTime, 0.0f);
		currentRotation = currentRotation * deltaRotation;
		transform->SetWorldRotation(currentRotation);
	}

	const Vector3D forward = Quaternion::Normalize(currentRotation) * Vector3D::GetForward();
	Vector3D movement = transform->GetWorldPosition();
	float moveInput = 0.0f;

	if (Input::IsKeyHeld(VK_UP))
	{
		moveInput += 1.0f;
	}
	if (Input::IsKeyHeld(VK_DOWN))
	{
		moveInput -= 1.0f;
	}
	movement += forward * (moveInput * moveSpeed * deltaTime);

	if (Input::IsKeyHeld(VK_SPACE))
	{
		movement += Vector3D::GetUp() * (moveSpeed * deltaTime);
	}
	if (Input::IsKeyHeld(VK_CONTROL))
	{
		movement += Vector3D::GetDown() * (moveSpeed * deltaTime);
	}

	transform->SetWorldPosition(movement);

	if (Input::IsKeyHeld('Z') && fireTimer <= Mathf::EPSILON)
	{
		fireTimer = fireCooldown;

		Scene* scene = GetOwner() ? GetOwner()->GetCurrentScene() : nullptr;
		if (!scene)
		{
			return;
		}

		const Vector3D muzzlePosition =
			transform->GetWorldPosition() +
			forward * bulletSpawnForwardOffset +
			Vector3D::GetUp() * bulletSpawnUpOffset;

		Object* bulletObject = scene->AddObject();
		bulletObject->SetName("Bullet");
		bulletObject->SetTag("Bullet");

		Transform* bulletTransform = bulletObject->GetTransform();
		bulletTransform->SetWorldPosition(muzzlePosition);
		bulletTransform->SetWorldRotation(currentRotation);
		bulletTransform->SetWorldScale(Vector3D(0.1f, 0.1f, 0.1f));

		MeshRenderer* bulletRenderer = bulletObject->AddComponent<MeshRenderer>();
		bulletRenderer->SetMesh(Mesh::LoadFromFile("./Resources/Sphere.obj"));
		
		CollisionSphere* collisionSphere = bulletObject->AddComponent<CollisionSphere>();
		if (collisionSphere)
		{
			collisionSphere->SetLocalCenter(Vector3D::GetZero());
			collisionSphere->SetLocalRadius(0.35f);
		}

		bulletObject->AddComponent<Bullet>();
	}
}
