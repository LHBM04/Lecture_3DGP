#include "Precompiled.h"
#include "PlayerInputMapper.h"

#include "InputManager.h"

PlayerInput PlayerInputMapper::Build() const noexcept
{
	PlayerInput input{};

	if (InputManager::IsKeyDown(KeyCode::A) || InputManager::IsKeyDown(KeyCode::Left))
	{
		input.yaw -= 1.0f;
	}
	if (InputManager::IsKeyDown(KeyCode::D) || InputManager::IsKeyDown(KeyCode::Right))
	{
		input.yaw += 1.0f;
	}
	if (InputManager::IsKeyDown(KeyCode::W) || InputManager::IsKeyDown(KeyCode::Up))
	{
		input.move += 1.0f;
	}
	if (InputManager::IsKeyDown(KeyCode::S) || InputManager::IsKeyDown(KeyCode::Down))
	{
		input.move -= 1.0f;
	}

	input.jumpPressed = InputManager::IsKeyPressed(KeyCode::Space);
	input.dodgePressed = InputManager::IsKeyPressed(KeyCode::Shift);
	return input;
}
