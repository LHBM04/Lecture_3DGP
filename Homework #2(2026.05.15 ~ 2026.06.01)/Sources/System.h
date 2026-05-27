#pragma once

class ISystem
{
public:
	ISystem() = default;
	~ISystem() = default;

	ISystem(const ISystem&) = delete;
	ISystem operator=(const ISystem&) = delete;

	ISystem(ISystem&&) = delete;
	ISystem operator=(ISystem&&) = delete;

	virtual void Release() = 0;
};