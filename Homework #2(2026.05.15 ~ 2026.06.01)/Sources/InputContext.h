#pragma once

#include "InputSystem.h"
#include "InputPhase.h"

class InputContext final
{
public:
	InputContext() = default;

	InputContext(KeyCode key_, InputPhase phase_) noexcept
		: key{ key_ }
		, phase{ phase_ }
	{
	}

	[[nodiscard]] KeyCode GetKey() const noexcept
	{
		return key;
	}

	[[nodiscard]] InputPhase GetPhase() const noexcept
	{
		return phase;
	}

	[[nodiscard]] bool IsKey(KeyCode key_) const noexcept
	{
		return key == key_;
	}

private:
	KeyCode key{ KeyCode::None };
	InputPhase phase{ InputPhase::Canceled };
};
