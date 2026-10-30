#pragma once

#include "Framework/Entities/World.hpp"

namespace TUK::Game
{
	class TestWorld : public Framework::World
	{
	public:
		TestWorld() noexcept = default;
		~TestWorld() noexcept override = default;

	protected:
		void OnEnter() override;

	};

}
