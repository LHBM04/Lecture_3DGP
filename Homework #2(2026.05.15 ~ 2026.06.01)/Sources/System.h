#pragma once

template <class TOptions>
class System
{
public:
	System() noexcept = default;
	virtual ~System() noexcept = default;

	// 복사 금지.
	System(const System&) = delete;
	System operator=(const System&) = delete;

	// 이동 금지.
	System(System&&) = delete;
	System operator=(System&&) = delete;

	virtual bool Initialize(const TOptions& options) = 0;
	virtual void Release() = 0;
};
