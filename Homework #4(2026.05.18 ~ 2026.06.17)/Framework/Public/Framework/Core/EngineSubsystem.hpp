#pragma once

#include "Subsystem.hpp"

namespace TUK::Framework
{
	class EngineSubsystem : public Subsystem
	{
		friend class Engine;

	public:
		~EngineSubsystem() noexcept override;

	protected:
		explicit EngineSubsystem(unsigned short priority) noexcept;

		virtual SystemStatus OnPreTick();
		virtual SystemStatus OnTick();
		virtual SystemStatus OnPostTick();
	};
}