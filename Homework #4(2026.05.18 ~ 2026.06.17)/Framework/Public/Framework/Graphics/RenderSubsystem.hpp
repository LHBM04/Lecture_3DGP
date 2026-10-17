#pragma once

#include <vector>
#include <map>

#include "../Core/EngineSubsystem.hpp"

#include "GraphicsDevice.hpp"
#include "SwapChain.hpp"

namespace TUK::Framework
{
	class Display;

	class RenderSubsystem : public EngineSubsystem
	{
	public:
		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override;

		void RegisterWindow(const Display& window);

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;
	};
}
