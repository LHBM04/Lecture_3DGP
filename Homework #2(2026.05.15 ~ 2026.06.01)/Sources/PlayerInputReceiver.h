#pragma once

#include "PlayerInput.h"

class PlayerInputReceiver
{
public:
	virtual ~PlayerInputReceiver() = default;
	virtual void OnPlayerInput(const PlayerInput& input_) = 0;
};
