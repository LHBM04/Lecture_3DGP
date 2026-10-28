#pragma once

#include "Framework/Core/App.hpp"

#include "Framework/Platform/Display.hpp"

using SystemStatus = TUK::Framework::SystemStatus;

namespace TUK::Game
{
	class MyApp : public Framework::App
	{
		DECLARE_APP_INSTANCE();

	public:
		MyApp() noexcept;
		~MyApp() noexcept override;

		SystemStatus Startup() override;
		void Shutdown() override;

	private:
		Framework::Display* window;
	};
}
