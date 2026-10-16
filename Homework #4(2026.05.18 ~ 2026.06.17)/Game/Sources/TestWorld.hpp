#pragma once

#include "Framework/Entities/World.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class TestWorld : public World
	{
	public:
		TestWorld() noexcept = default;
		~TestWorld() noexcept override = default;

	protected:
		void OnEnter() override;
		void OnRender(CommandBuffer& renderer) override;
		void OnExit() override;
	};
}
