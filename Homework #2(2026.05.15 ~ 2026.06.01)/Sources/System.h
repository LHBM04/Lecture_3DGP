#pragma once

class System
{
public:
	System() noexcept = default;
	virtual ~System() noexcept = default;

	System(const System&) = delete;
	System& operator=(const System&) = delete;

	System(System&&) = delete;
	System& operator=(System&&) = delete;

	virtual void Release() = 0;
};
