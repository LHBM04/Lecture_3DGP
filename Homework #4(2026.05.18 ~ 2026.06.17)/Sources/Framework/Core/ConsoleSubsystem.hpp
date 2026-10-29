#pragma once

#include "EngineSubsystem.hpp"

namespace TUK::Framework
{
	class ConsoleSubsystem : public EngineSubsystem
	{
		friend class System;

	public:
		~ConsoleSubsystem() noexcept override;

	protected:
		SystemStatus OnStartup() override;
		void OnShutdown() override;

	private:
		ConsoleSubsystem() noexcept;
	};
}
