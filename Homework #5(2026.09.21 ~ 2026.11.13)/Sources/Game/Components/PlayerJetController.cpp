#include "Precompiled.hpp"
#include "PlayerJetController.hpp"

#include "ProjectileController.hpp"

#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Input.hpp"
#include "../Core/Time.hpp"

namespace TUK::Game
{
	void PlayerJetController::OnStart()
	{
		if (InputAction* moveAction = Input::GetAction("Move"))
		{
			moveListenerId = moveAction->OnPerformed().AddListener(std::bind_front(&PlayerJetController::OnMovePerformed, this));
		}

		projectileMesh = Assets::LoadAsset<MeshAsset>("Resources/Meshes/SM_Bullet_01/SM_Bullet_01.tukmesh");
		projectileMaterial = Assets::LoadAsset<MaterialAsset>("Resources/Materials/Bullets_M.tukmat");
	}

	void PlayerJetController::OnUpdate()
	{
		const Vector2D<i32> mouseDelta = Input::GetMousePositionDelta();
		if (mouseDelta.GetX() == 0 && mouseDelta.GetY() == 0)
		{
			return;
		}

		GameObject* owner = GetOwner();
		ASSERT_FMT(owner, "PlayerJetController 컴포넌트가 부착된 게임 오브젝트를 찾을 수 없습니다.");

		Transform* transform = owner->GetComponent<Transform>();
		if (!transform)
		{
			LOG_ERROR("유효한 Transform 컴포넌트를 찾을 수 없습니다.");
			return;
		}

		const f32 pitch = static_cast<f32>(mouseDelta.GetY()) * rotationSpeed;
		const f32 yaw = static_cast<f32>(mouseDelta.GetX()) * rotationSpeed;
		const Quaternion<f32> rotation = Quaternion<f32>::Euler(pitch, yaw, 0.0f) * transform->GetWorldRotation();
		transform->SetWorldRotation(rotation);
	}

	void PlayerJetController::OnLateUpdate()
	{
		const Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			return;
		}

		if (Input::IsMouseButtonPressed(ButtonCode::LEFT))
		{
			Fire(transform->GetWorldRotation() * Vector3D<f32>::GetForward());
		}
	}

	void PlayerJetController::OnDestroy()
	{
		if (moveListenerId != -1)
		{
			if (InputAction* moveAction = Input::GetAction("Move"))
			{
				moveAction->OnPerformed().RemoveListener(moveListenerId);
			}
			moveListenerId = -1;
		}
	}

	void PlayerJetController::OnMovePerformed(const InputActionContext& context)
	{
		GameObject* owner = GetOwner();
		ASSERT_FMT(owner, "PlayerJetController 컴포넌트가 부착된 게임 오브젝트를 찾을 수 없습니다.");

		Transform* transform = owner->GetComponent<Transform>();
		if (!transform)
		{
			LOG_ERROR("유효한 Transform 컴포넌트를 찾을 수 없습니다.");
			return;
		}

		const Vector2D<f32>& axis = context.value;
		const f32 deltaTime = Time::GetDeltaTime();

		Vector3D<f32> direction(axis.GetX(), 0.0f, axis.GetY());
		if (direction.GetSqrMagnitude() > 1.0f)
		{
			direction.Normalize();
		}

		const f32 speedMultiplier = Input::IsKeyHeld(KeyCode::LEFT_SHIFT) ? boostMultiplier : 1.0f;
		const Quaternion<f32> rotation = transform->GetWorldRotation();
		const Vector3D<f32> movement = rotation * direction * (moveSpeed * speedMultiplier * deltaTime);
		transform->SetWorldPosition(transform->GetWorldPosition() + movement);
	}

	void PlayerJetController::Fire(const Vector3D<f32>& direction)
	{
		Scene* scene = GetOwner()->GetScene();
		const Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!scene || !transform || !projectileMesh || !projectileMaterial)
		{
			return;
		}

		const Vector3D<f32> position = transform->GetWorldPosition() + transform->GetWorldRotation() * muzzleOffset;
		const Vector3D<f32> up = Vector3D<f32>::GetUp();
		const Vector3D<f32> shotUp = std::abs(direction.Dot(up)) > 0.99f ? Vector3D<f32>::GetForward() : up;
		const Quaternion<f32> aimRotation = Quaternion<f32>::LookRotation(direction, shotUp);
		const Vector3D<f32> shotDirection = aimRotation * Vector3D<f32>::GetForward();
		const Quaternion<f32> projectileRotation = Quaternion<f32>::Euler(90.0f, 0.0f, 0.0f) * aimRotation;

		GameObject* projectile = scene->Instantiate(position, projectileRotation);
		projectile->SetName("Player Projectile");
		projectile->SetTag("PlayerProjectile");
		projectile->GetComponent<Transform>()->SetLocalScale(Vector3D<f32>(5.0f, 5.0f, 5.0f));

		MeshRenderer* renderer = projectile->AddComponent<MeshRenderer>();
		renderer->SetMesh(projectileMesh);
		renderer->SetMaterial(projectileMaterial);
		renderer->SetColor(ColorRGBA<f32>(0.2f, 0.8f, 1.0f, 1.0f));

		ProjectileController* controller = projectile->AddComponent<ProjectileController>();
		controller->SetLaunchPosition(position);
		controller->SetLaunchDirection(shotDirection);
		controller->SetMoveSpeed(projectileSpeed);
		controller->SetLifetime(projectileLifetime);
		controller->SetTargetTag("Enemy");
	}
}
