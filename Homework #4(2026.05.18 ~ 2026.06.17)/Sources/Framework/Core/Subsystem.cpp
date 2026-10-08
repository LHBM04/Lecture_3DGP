#include "Precompiled.hpp"
#include "Subsystem.hpp"

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
