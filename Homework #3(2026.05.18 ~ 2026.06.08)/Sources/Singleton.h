#pragma once

template <class TInstance>
class Singleton
{
public:
	Singleton() noexcept = default;
	virtual ~Singleton() noexcept = default;

	[[nodiscard]] static TInstance& GetInstance();

private:
	// 복사 금지.
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	// 이동 금지.
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;
};

template <class TInstance>
inline TInstance& Singleton<TInstance>::GetInstance()
{
	static TInstance instance{};
	return instance;
}
