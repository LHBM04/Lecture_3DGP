#pragma once

template <class TInstance>
class Singleton
{
public:
	Singleton() = default;
	virtual ~Singleton() = default;

	// 복사 금지.
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	// 이동 금지.
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	[[nodiscard]] static TInstance& GetInstance() noexcept;
};

template <class TInstance>
TInstance& Singleton<TInstance>::GetInstance() noexcept
{
	static TInstance* instance{ new TInstance() };
	return *instance;
}
