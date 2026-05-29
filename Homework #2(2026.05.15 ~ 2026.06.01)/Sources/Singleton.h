#pragma once

template <class TInstance>
class Singleton
{
protected:
	Singleton() = default;
	virtual ~Singleton() = default;

	// 복사 금지.
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	// 이동 금지.
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

public:
	[[nodiscard]] static TInstance& GetInstance();
};

template<class TInstance>
TInstance& Singleton<TInstance>::GetInstance()
{
	static TInstance* instance = new TInstance();
	return *instance;
}
