#pragma once

#include "Common.hpp"
#include "SystemStatus.hpp"

namespace TUK::Framework
{
	class Subsystem
	{
	public:
		explicit Subsystem(u8 priority) noexcept;
		virtual ~Subsystem() noexcept = default;

		Subsystem(const Subsystem&) = delete;
		Subsystem& operator=(const Subsystem&) = delete;

		Subsystem(Subsystem&&) = delete;
		Subsystem& operator=(Subsystem&&) = delete;

		virtual SystemStatus OnStartup() = 0;
		virtual void OnShutdown() = 0;

		[[nodiscard]] u8 GetPriority() const noexcept;

	private:
		u8 priority;
	};

	inline Subsystem::Subsystem(u8 priority) noexcept
		: priority(priority)
	{
	}

	inline u8 Subsystem::GetPriority() const noexcept
	{
		return priority;
	}
}
