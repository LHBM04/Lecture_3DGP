#pragma once

#include "Common.hpp"
#include "Subsystem.hpp"

namespace TUK::Framework
{
	class EngineSubsystem : public Subsystem
	{
	public:
		using Subsystem::Subsystem;
		~EngineSubsystem() noexcept override = default;

		virtual SystemStatus OnPreTick() = 0;
		virtual SystemStatus OnTick() = 0;
		virtual SystemStatus OnPostTick() = 0;
	};
}
