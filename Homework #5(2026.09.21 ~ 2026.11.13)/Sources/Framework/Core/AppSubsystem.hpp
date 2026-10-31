#pragma once

#include "Common.hpp"
#include "Subsystem.hpp"

namespace TUK::Framework
{
	class AppSubsystem : public Subsystem
	{
	public:
		explicit AppSubsystem(u8 priority) noexcept;
		~AppSubsystem() noexcept override = default;

		virtual SystemStatus OnEarlyUpdate() = 0;
		virtual SystemStatus OnFixedUpdate() = 0;
		virtual SystemStatus OnUpdate() = 0;
		virtual SystemStatus OnLateUpdate() = 0;
	};

	inline AppSubsystem::AppSubsystem(u8 priority) noexcept
		: Subsystem(priority)
	{
	}
}
