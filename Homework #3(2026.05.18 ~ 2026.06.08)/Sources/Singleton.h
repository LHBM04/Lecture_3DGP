#pragma once

template <class TInstance>
class Singleton
{
public:
	Singleton() = default;
	virtual ~Singleton() = default;

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	[[nodiscard]] static TInstance& GetInstance() noexcept;
};

template <class TInstance>
inline TInstance& Singleton<TInstance>::GetInstance() noexcept
{
	static TInstance* instance{ new TInstance() };
	return *instance;
}
