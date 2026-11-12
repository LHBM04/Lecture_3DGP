#pragma once

#include "Framework/Core.hpp"
#include "Framework/GameObjects.hpp"
#include "Framework/Platform.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class PlayerTankController : public Component
	{
		using Component::Component;

	public:
		void OnStart() override;
		void OnDestroy() override;

	private:
		void OnMovePerformed(const InputActionContext& context);

		static constexpr f32 DEFAULT_MOVE_SPEED = 5.0f;
		static constexpr f32 DEFAULT_ROTATION_SPEED = 90.0f;

		f32 moveSpeed = DEFAULT_MOVE_SPEED;
		f32 rotationSpeed = DEFAULT_ROTATION_SPEED;

		InputAction::ActionCallback::ListenerId moveListenerId = -1;
	};
}
