#include "Precompiled.hpp"
#include "PlayerController.hpp"

#include "../Time.hpp"
#include "../Input.hpp"

namespace TUK::Game
{
	void PlayerController::OnFixedUpdate()
	{
		Component::OnFixedUpdate();
	}

	void PlayerController::OnUpdate()
	{
		Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			LOG_ERROR("유효한 Transform 컴포넌트를 찾을 수 없습니다.");
			return;
		}

		Vector3D<f32> movement = transform->GetWorldPosition();
		if (Input::IsKeyHeld(KeyCode::UP))
		{
			movement += Vector3D<f32>::GetForward() * moveSpeed * Time::GetDeltaTime();
		}
		if (Input::IsKeyHeld(KeyCode::DOWN))
		{
			movement -= Vector3D<f32>::GetForward() * moveSpeed * Time::GetDeltaTime();
		}

		Quaternion<f32> rotation = transform->GetWorldRotation();
		if (Input::IsKeyHeld(KeyCode::LEFT))
		{
			rotation *= Quaternion<f32>::Euler(Vector3D<f32>::GetUp()) * rotationSpeed * Time::GetDeltaTime();
		}
		if (Input::IsKeyHeld(KeyCode::RIGHT))
		{
			rotation *= Quaternion<f32>::Euler(Vector3D<f32>::GetDown()) * rotationSpeed * Time::GetDeltaTime();
		}

		transform->SetWorldPosition(movement);
		transform->SetWorldRotation(rotation);
	}
}
