#pragma once

#include "Framework/Core/Engine.hpp"

using SystemStatus = TUK::Framework::SystemStatus;

namespace TUK::Game
{
	class MyEngine : public Framework::Engine
	{
		DECLARE_ENGINE_INSTANCE();

	public:
		MyEngine() noexcept;
		~MyEngine() noexcept override;

		SystemStatus Ready() override;
	};
}
