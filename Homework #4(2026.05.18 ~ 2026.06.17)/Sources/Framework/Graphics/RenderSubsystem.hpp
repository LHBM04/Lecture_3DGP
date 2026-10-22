#pragma once

#include <functional>
#include <map>
#include <vector>

#include "../Core/EngineSubsystem.hpp"

#include "GraphicsDevice.hpp"
#include "SwapChain.hpp"

namespace TUK::Framework
{
	class Display;

	class RenderSubsystem : public EngineSubsystem
	{
	public:
		struct RenderCallbacks final
		{
			std::function<void(CommandBuffer&)> preRender;
			std::function<void(CommandBuffer&)> render;
			std::function<void(CommandBuffer&)> postRender;
		};

		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override;

		void RegisterDisplay(const Display& window);
		void UnregisterDisplay(const Display& window);

		void SetRenderCallback(const Display& window, std::function<void(CommandBuffer&)> callback);
		void SetRenderCallbacks(const Display& window, RenderCallbacks callbacks);

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

	private:
		void RemovePendingDisplays();

		GraphicsDevice device;
		std::map<const Display*, SwapChain> swapChains;
		std::map<const Display*, RenderCallbacks> renderCallbacks;
		std::vector<const Display*> pendingDisplayRemovals;
		CommandBuffer* activeCommandBuffer = nullptr;
	};
}
