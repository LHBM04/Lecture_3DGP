#include "Precompiled.hpp"
#include "Framework/Graphics/RenderSubsystem.hpp"

namespace TUK::Framework
{
	RenderSubsystem::RenderSubsystem() noexcept
		: EngineSubsystem(100)
	{
	}
	
	RenderSubsystem::~RenderSubsystem() noexcept
	{
	}

	void RenderSubsystem::RegisterWindow(const Display& window)
	{
	}
	
	SystemStatus RenderSubsystem::OnStartup()
	{
		return SystemStatus::Continue;
	}
	
	SystemStatus RenderSubsystem::OnPreTick()
	{
		return SystemStatus::Continue;
	}
	
	SystemStatus RenderSubsystem::OnPostTick()
	{
		return SystemStatus::Continue;
	}
	
	void RenderSubsystem::OnShutdown()
	{
	}
}
