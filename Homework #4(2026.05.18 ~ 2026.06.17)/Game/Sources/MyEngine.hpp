#pragma once

#include "Framework/Core/Engine.hpp"

namespace TUK::Game
{
	class MyEngine : public Framework::Engine
	{
		DECLARE_ENGINE_INSTANCE();

	public:
		void Ready() override;

	private:
		MyEngine() noexcept;
		~MyEngine() noexcept override;
	};
}
