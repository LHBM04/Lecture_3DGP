#pragma once

#include "PlayerInput.h"

class PlayerInputMapper final
{
public:
	[[nodiscard]] PlayerInput Build() const noexcept;
};
