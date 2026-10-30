#pragma once

#include <cstdint>

#include "Subsystem.hpp"

namespace TUK::Framework
{
	class AppSubsystem : public Subsystem
	{
		friend class App;

	public:
		~AppSubsystem() noexcept override;

	protected:
		explicit AppSubsystem(std::uint16_t priority) noexcept;

		virtual SystemStatus OnEarlyUpdate();
		virtual SystemStatus OnFixedUpdate();
		virtual SystemStatus OnUpdate();
		virtual SystemStatus OnLateUpdate();
	};

	inline AppSubsystem::AppSubsystem(std::uint16_t priority) noexcept
		: Subsystem(priority)
	{
	}

	inline SystemStatus AppSubsystem::OnEarlyUpdate()
	{
		return SystemStatus::Continue;
	}

	inline SystemStatus AppSubsystem::OnFixedUpdate()
	{
		return SystemStatus::Continue;
	}

	inline SystemStatus AppSubsystem::OnUpdate()
	{
		return SystemStatus::Continue;
	}

	inline SystemStatus AppSubsystem::OnLateUpdate()
	{
		return SystemStatus::Continue;
	}
}
