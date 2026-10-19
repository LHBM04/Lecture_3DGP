#pragma once

#include "EngineSubsystem.hpp"

namespace TUK::Framework
{
	class ConsoleSubsystem : public EngineSubsystem
	{
	public:
		ConsoleSubsystem() noexcept;
		~ConsoleSubsystem() noexcept override;

	protected:
		SystemStatus OnStartup() override;
		void OnShutdown() override;
	};
}
