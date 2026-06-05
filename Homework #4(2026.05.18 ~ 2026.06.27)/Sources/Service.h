#pragma once

class Framework;

class Service
{
public:
	Service() noexcept = default;
	virtual ~Service() noexcept = default;

	void NotifyAdd(Framework& framework_);
	void NotifyRemove();

	[[nodiscard]] Framework& GetFramework() const noexcept;

protected:
	virtual void OnAdd() {};
	virtual void OnRemove() {};

private:
	Service(const Service&) = delete;
	Service& operator=(const Service&) = delete;

	Service(Service&&) = delete;
	Service& operator=(Service&&) = delete;

	Framework* framework{ nullptr };
};
