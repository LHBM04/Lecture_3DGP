#pragma once

class Framework;

class Service
{
public:
	Service() noexcept = default;
	virtual ~Service() noexcept = default;

	Service(const Service&) noexcept = delete;
	Service& operator=(const Service&) noexcept = delete;

	Service(Service&&) noexcept = delete;
	Service& operator=(Service&&) noexcept = delete;

	void NotifyAdd(Framework* framework_);
	void NotifyRemove();

	[[nodiscard]] Framework* GetFramework() const noexcept;

protected:
	virtual void OnAdd() {};
	virtual void OnRemove() {};

private:
	Framework* framework;
};