#pragma once

template <class TInstance>
class Singleton
{
protected:
	Singleton() = default;
	virtual ~Singleton() = default;

	// 복사 금지.
	Singleton(const Singleton&) = delete;
	void operator=(const Singleton&) = delete;

	// 이동 금지.
	Singleton(Singleton&&) = delete;
	void operator=(Singleton&&) = delete;

public:
	[[nodiscard]] inline static TInstance& GetInstance();
};

template <class TInstance>
inline TInstance& Singleton<TInstance>::GetInstance()
{
	static TInstance* const instance{ new TInstance() };
	return *instance;
}
