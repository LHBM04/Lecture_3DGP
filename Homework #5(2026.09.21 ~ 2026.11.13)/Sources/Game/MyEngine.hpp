#pragma once

#include "../Framework/Core/Engine.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class MyEngine : public Engine
	{
	public:
		explicit MyEngine(App& app) noexcept;
		~MyEngine() noexcept override;

		SystemStatus Startup() override;
	};

	inline MyEngine::MyEngine(App& app) noexcept
		: Engine(app)
	{
	}

	inline MyEngine::~MyEngine() noexcept
	{
	}
}
