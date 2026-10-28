#pragma once

#include <cstdint>

#include "Subsystem.hpp"

namespace TUK::Framework
{
	class EngineSubsystem : public Subsystem
	{
		friend class Engine;

	public:
		~EngineSubsystem() noexcept override;

	protected:
		explicit EngineSubsystem(std::uint16_t priority) noexcept;

		virtual SystemStatus OnPreTick();
		virtual SystemStatus OnTick();
		virtual SystemStatus OnPostTick();
	};

	inline EngineSubsystem::EngineSubsystem(std::uint16_t priority) noexcept
		: Subsystem(priority)
	{
	}

	inline SystemStatus EngineSubsystem::OnPreTick()
	{
		return SystemStatus::Continue;
	}

	inline SystemStatus EngineSubsystem::OnTick()
	{
		return SystemStatus::Continue;
	}

	inline SystemStatus EngineSubsystem::OnPostTick()
	{
		return SystemStatus::Continue;
	}
}
