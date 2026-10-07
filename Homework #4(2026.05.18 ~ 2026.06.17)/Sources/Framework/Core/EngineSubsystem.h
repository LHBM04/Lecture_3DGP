#pragma once

#include "Subsystem.h"

namespace TUK::Framework
{
	class EngineSubsystem : public Subsystem
	{
		friend class Engine;

	public:
		explicit EngineSubsystem(unsigned short priority) noexcept;
		~EngineSubsystem() noexcept override;

	protected:
		virtual void OnPreTick();
		virtual void OnTick();
		virtual void OnPostTick();
	};
}