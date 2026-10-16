#pragma once

#include <vector>
#include <map>

#include "../Core/EngineSubsystem.hpp"

#include "GraphicsDevice.hpp"
#include "SwapChain.hpp"

namespace TUK::Framework
{
	class Window;
	class WindowSubsystem;

	class RenderSubsystem : public EngineSubsystem
	{
	public:
		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override;

		void RegisterWindow(const Window& window);

	protected:
		void OnStartup() override;
		void PreTick() override;
		void PostTick() override;
		void OnShutdown() override;

	private:
		
	};
}
