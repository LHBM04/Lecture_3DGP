#pragma once

namespace TUK::Framework
{
	class Fence
	{
	public:
		virtual ~Fence() noexcept;

		Fence(const Fence&) = delete;
		Fence& operator=(const Fence&) = delete;

		Fence(Fence&&) = delete;
		Fence& operator=(Fence&&) = delete;

		virtual void Wait() const = 0;

		[[nodiscard]] virtual bool IsCompleted() const noexcept = 0;

	protected:
		Fence() noexcept;
	};
}
