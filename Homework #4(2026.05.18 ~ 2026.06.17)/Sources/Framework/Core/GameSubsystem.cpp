#include "Precompiled.hpp"
#include "GameSubsystem.hpp"

namespace TUK::Framework
{
	GameSubsystem::GameSubsystem(unsigned short priority) noexcept
		: Subsystem(priority)
	{
	}

	GameSubsystem::~GameSubsystem() noexcept
	{
	}

	void GameSubsystem::EarlyUpdate(double)
	{
	}
	void GameSubsystem::FixedUpdate(double)
	{
	}
	void GameSubsystem::Update(double)
	{
	}
	void GameSubsystem::LateUpdate(double)
	{
	}

}