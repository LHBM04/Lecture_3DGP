#include "Precompiled.hpp"
#include "Framework/Core/Subsystem.hpp"

namespace TUK::Framework
{
	Subsystem::Subsystem(std::uint16_t priority) noexcept
		: priority(priority)
	{
	}

	Subsystem::~Subsystem() noexcept = default;

	std::uint16_t Subsystem::GetPriority() const noexcept
	{
		return priority;
	}

	SystemStatus Subsystem::OnStartup()
	{
		return SystemStatus::Continue;
	}

	void Subsystem::OnShutdown()
	{
	}
}
