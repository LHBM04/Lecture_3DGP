#include "Precompiled.h"
#include "Subsystem.h"

namespace TUK::Framework
{
	Subsystem::Subsystem(unsigned short priority) noexcept
		: priority(priority)
	{
	}

	Subsystem::~Subsystem() noexcept = default;

	unsigned short Subsystem::GetPriority() const noexcept
	{
		return priority;
	}

	void Subsystem::OnStartup()
	{
	}

	void Subsystem::OnShutdown()
	{
	}
}
