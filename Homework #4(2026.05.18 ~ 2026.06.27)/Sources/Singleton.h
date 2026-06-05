#pragma once

template <class TInstance>
class Singleton
{
public:
	Singleton() noexcept = default;
	virtual ~Singleton() noexcept = default;

	[[nodiscard]] static TInstance& GetInstance();

private:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;
};

template <class TInstance>
inline TInstance& Singleton<TInstance>::GetInstance()
{
	static TInstance instance{};
	return instance;
}
