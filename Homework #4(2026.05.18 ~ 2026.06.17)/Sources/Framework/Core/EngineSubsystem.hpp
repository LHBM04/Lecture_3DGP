#pragma once

#include <cstdint>

#include "Framework/Core/Subsystem.hpp"

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
}
