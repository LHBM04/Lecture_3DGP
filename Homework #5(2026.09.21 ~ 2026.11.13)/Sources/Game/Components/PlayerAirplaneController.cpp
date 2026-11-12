#include "Precompiled.hpp"
#include "PlayerAirplaneController.hpp"

#include "../Core/Input.hpp"
#include "../Core/Time.hpp"

namespace TUK::Game
{
	void PlayerAirplaneController::OnStart()
	{
		if (InputAction* moveAction = Input::GetAction("Move"))
		{
			moveListenerId = moveAction->OnPerformed().AddListener(std::bind_front(&PlayerAirplaneController::OnMovePerformed, this));
		}
	}

	void PlayerAirplaneController::OnDestroy()
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

	void PlayerAirplaneController::OnMovePerformed(const InputActionContext& context)
	{
		const GameObject* owner = GetOwner();
		ASSERT_FMT(owner, "PlayerAirplaneController 컴포넌트가 부착된 게임 오브젝트를 찾을 수 없습니다.");

		Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			LOG_ERROR("유효한 Transform 컴포넌트를 찾을 수 없습니다.");
			return;
		}

		const Vector2D<f32>& axis = context.value;
		const f32 deltaTime = Time::GetDeltaTime();

		Vector3D<f32> movement = transform->GetWorldPosition();
		movement += Vector3D<f32>::GetForward() * (axis.GetY() * moveSpeed * deltaTime);

		Quaternion<f32> rotation = transform->GetWorldRotation();
		rotation *= Quaternion<f32>::Euler(Vector3D<f32>::GetDown() * (axis.GetX() * rotationSpeed * deltaTime));

		transform->SetWorldPosition(movement);
		transform->SetWorldRotation(rotation);
	}
}
