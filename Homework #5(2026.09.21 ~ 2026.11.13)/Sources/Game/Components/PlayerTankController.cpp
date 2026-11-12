#include "Precompiled.hpp"
#include "PlayerTankController.hpp"

#include "ProjectileController.hpp"
#include "TerrainAlignment.hpp"

#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Input.hpp"
#include "../Core/Time.hpp"

namespace TUK::Game
{
	void PlayerTankController::OnStart()
	{
		projectileMesh = Assets::LoadAsset<MeshAsset>("Resources/Meshes/SM_Bullet_01/SM_Bullet_01.tukmesh");
		projectileMaterial = Assets::LoadAsset<MaterialAsset>("Resources/Materials/Bullets_M.tukmat");

		if (GameObject* owner = GetOwner())
		{
			if (Transform* transform = owner->GetComponent<Transform>())
			{
				yawRotation = ExtractHorizontalYawRotation(transform->GetWorldRotation());
				Vector3D<f32> position = transform->GetWorldPosition();
				const Quaternion<f32> alignedRotation = AlignTankToTerrain(
					owner->GetScene(),
					position,
					smoothedUp,
					yawRotation * Vector3D<f32>::GetForward(),
					1.0f);
				transform->SetWorldPosition(position);
				transform->SetWorldRotation(alignedRotation);
			}
		}
	}

	void PlayerTankController::OnUpdate()
	{
		GameObject* owner = GetOwner();
		ASSERT_FMT(owner, "PlayerTankController 컴포넌트가 부착된 게임 오브젝트를 찾을 수 없습니다.");

		Transform* transform = owner->GetComponent<Transform>();
		if (!transform)
		{
			LOG_ERROR("유효한 Transform 컴포넌트를 찾을 수 없습니다.");
			return;
		}

		const InputAction* moveAction = Input::GetAction("Move");
		const Vector2D<f32> axis = moveAction ? moveAction->GetValue() : Vector2D<f32>{};
		const Vector2D<i32> mouseDelta = Input::GetMousePositionDelta();
		const f32 deltaTime = Time::GetDeltaTime();

		yawRotation *= Quaternion<f32>::Euler(Vector3D<f32>::GetDown() * (axis.GetX() * rotationSpeed * deltaTime));
		yawRotation *= Quaternion<f32>::Euler(Vector3D<f32>::GetUp() * (static_cast<f32>(mouseDelta.GetX()) * mouseSensitivity));
		yawRotation.Normalize();

		const f32 speedMultiplier = Input::IsKeyHeld(KeyCode::LEFT_SHIFT) ? boostMultiplier : 1.0f;
		Vector3D<f32> movement = transform->GetWorldPosition();
		movement += (transform->GetWorldRotation() * Vector3D<f32>::GetForward()) * (axis.GetY() * moveSpeed * speedMultiplier * deltaTime);

		const f32 blendFactor = ComputeTiltBlendFactor(tiltAlignSpeed, deltaTime);
		const Quaternion<f32> alignedRotation = AlignTankToTerrain(
			owner->GetScene(),
			movement,
			smoothedUp,
			yawRotation * Vector3D<f32>::GetForward(),
			blendFactor);

		transform->SetWorldPosition(movement);
		transform->SetWorldRotation(alignedRotation);
	}

	void PlayerTankController::OnLateUpdate()
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

	void PlayerTankController::Fire(const Vector3D<f32>& direction)
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
