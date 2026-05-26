#pragma once

struct PlayerInput final
{
	float yaw{ 0.0f };
	float move{ 0.0f };
	bool jumpPressed{ false };
	bool dodgePressed{ false };
};
