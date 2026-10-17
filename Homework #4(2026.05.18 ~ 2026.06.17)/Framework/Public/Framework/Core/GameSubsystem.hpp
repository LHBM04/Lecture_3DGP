#pragma once

#include "Subsystem.hpp"

namespace TUK::Framework
{
	class GameSubsystem : public Subsystem
	{
		friend class Game;

	public:
		explicit GameSubsystem(unsigned short priority) noexcept;
		~GameSubsystem() noexcept override;

	protected:
		virtual SystemStatus OnEarlyUpdate();
		virtual SystemStatus OnFixedUpdate();
		virtual SystemStatus OnUpdate();
		virtual SystemStatus OnLateUpdate();
	};
}
