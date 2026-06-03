#pragma once

class Engine;

class Service
{
public:
	Service() = default;
	virtual ~Service() = default;

	void NotifyAdd(Engine* engine_);
	void NotifyRemove();

protected:
	virtual void OnAdd() = 0;
	virtual void OnRemove() = 0;

	[[nodiscard]] Engine& GetEngine() const noexcept;

private:
	Service(const Service&) = delete;
	Service& operator=(const Service&) = delete;

	Service(Service&&) = delete;
	Service& operator=(Service&&) = delete;

	Engine* engine;
};