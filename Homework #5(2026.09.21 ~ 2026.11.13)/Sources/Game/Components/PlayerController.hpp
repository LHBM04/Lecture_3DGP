#pragma once

#include "Framework/Core.hpp"
#include "Framework/GameObjects.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class PlayerController : public Component
	{
	public:
		using Component::Component;

		void OnFixedUpdate() override;
		void OnUpdate() override;

	private:
		static constexpr f32 DefaultMoveSpeed = 5.0f;
		f32 moveSpeed = 5.0f;

		static constexpr f32 DefaultRotationSpeed = 90.0f;
		f32 rotationSpeed = 90.0f;
	};
}
