#pragma once

class Service
{
public:
	Service() noexcept = default;
	virtual ~Service() noexcept = default;

	virtual void OnAdd() {};
	virtual void OnRemove() {};

private:
	Service(const Service&) = delete;
	Service& operator=(const Service&) = delete;

	Service(Service&&) = delete;
	Service& operator=(Service&&) = delete;
};
