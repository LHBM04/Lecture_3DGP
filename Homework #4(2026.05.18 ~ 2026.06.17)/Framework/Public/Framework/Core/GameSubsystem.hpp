#pragma once

#include "Subsystem.hpp"

namespace TUK::Framework
{
	class CommandBuffer;

	class GameSubsystem : public Subsystem
	{
		friend class Game;

	public:
		explicit GameSubsystem(unsigned short priority) noexcept;
		~GameSubsystem() noexcept override;

	protected:
		virtual void EarlyUpdate();
		virtual void FixedUpdate();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Render(CommandBuffer& renderer);
	};
}
