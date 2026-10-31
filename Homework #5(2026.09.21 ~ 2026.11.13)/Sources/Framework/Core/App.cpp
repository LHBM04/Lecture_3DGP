#include "Precompiled.hpp"
#include "App.hpp"

namespace TUK::Framework
{
	SystemStatus App::Startup()
	{
		instance = nullptr;
		return System::Startup();
	}

	SystemStatus App::Routine()
	{
		if (SystemStatus status = EarlyUpdate(); status != SystemStatus::CONTINUE)
		{
			if (status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		if (SystemStatus status = FixedUpdate(); status != SystemStatus::CONTINUE)
		{
			if (status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		if (SystemStatus status = Update(); status != SystemStatus::CONTINUE)
		{
			if (status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		if (SystemStatus status = LateUpdate(); status != SystemStatus::CONTINUE)
		{
			if (status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		return SystemStatus::CONTINUE;
	}

	void App::Shutdown()
	{
		System::Shutdown();
		instance = nullptr;
	}

	SystemStatus App::EarlyUpdate()
	{
		for (AppSubsystem& subsystem : GetSubsystems())
		{
			if (const SystemStatus status = subsystem.OnEarlyUpdate(); status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus App::FixedUpdate()
	{
		for (AppSubsystem& subsystem : GetSubsystems())
		{
			if (const SystemStatus status = subsystem.OnFixedUpdate(); status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus App::Update()
	{
		for (AppSubsystem& subsystem : GetSubsystems())
		{
			if (const SystemStatus status = subsystem.OnUpdate(); status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus App::LateUpdate()
	{
		for (AppSubsystem& subsystem : GetSubsystems())
		{
			if (const SystemStatus status = subsystem.OnLateUpdate(); status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		return SystemStatus::CONTINUE;
	}
}
