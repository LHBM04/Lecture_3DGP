#include "Precompiled.hpp"
#include "App.hpp"

#include "../Platform/TimeSubsystem.hpp"

#include "Engine.hpp"

namespace TUK::Framework
{
	App::App() noexcept
	{
		assert(!instance);
		instance = this;
	}

	App::~App() noexcept
	{
		instance = nullptr;
	}

	SystemStatus App::Routine()
	{
		SystemStatus next = SystemStatus::Continue;

		if (SystemStatus current = EarlyUpdate(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}

		if (SystemStatus current = FixedUpdate(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}

		if (SystemStatus current = Update(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}
		
		if (SystemStatus current = LateUpdate(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}

		return next;
	}

	SystemStatus App::EarlyUpdate()
	{
		for (AppSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnEarlyUpdate();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Continue;
	}

	SystemStatus App::FixedUpdate()
	{
		for (AppSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnFixedUpdate();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Continue;
	}

	SystemStatus App::Update()
	{
		for (AppSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnUpdate();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Continue;
	}

	SystemStatus App::LateUpdate()
	{
		for (AppSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnLateUpdate();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Continue;
	}

	App* App::GetInstance()
	{
		assert(instance);
		return instance;
	}

	App* App::instance = nullptr;
}
