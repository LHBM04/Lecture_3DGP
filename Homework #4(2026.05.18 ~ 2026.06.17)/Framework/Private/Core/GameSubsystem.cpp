#include "Precompiled.hpp"
#include "Framework/Core/GameSubsystem.hpp"

namespace TUK::Framework
{
	GameSubsystem::GameSubsystem(unsigned short priority) noexcept
		: Subsystem(priority)
	{
	}

	GameSubsystem::~GameSubsystem() noexcept
	{
	}

	void GameSubsystem::EarlyUpdate()
	{
	}

	void GameSubsystem::FixedUpdate()
	{
	}

	void GameSubsystem::Update()
	{
	}

	void GameSubsystem::LateUpdate()
	{
	}

	void GameSubsystem::Render(CommandBuffer&)
	{
	}
}
