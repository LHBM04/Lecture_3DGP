#pragma once

#include <cstdint>

#include "Framework/Core/Subsystem.hpp"

namespace TUK::Framework
{
	class GameSubsystem : public Subsystem
	{
		friend class Game;

	public:
		explicit GameSubsystem(std::uint16_t priority) noexcept;
		~GameSubsystem() noexcept override;

	protected:
		virtual SystemStatus OnEarlyUpdate();
		virtual SystemStatus OnFixedUpdate();
		virtual SystemStatus OnUpdate();
		virtual SystemStatus OnLateUpdate();
	};
}
