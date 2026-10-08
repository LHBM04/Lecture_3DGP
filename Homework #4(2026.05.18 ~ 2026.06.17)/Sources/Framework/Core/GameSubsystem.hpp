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
		virtual void EarlyUpdate(double deltaTime);
		virtual void FixedUpdate(double deltaTime);
		virtual void Update(double deltaTime);
		virtual void LateUpdate(double deltaTime);
	};
}