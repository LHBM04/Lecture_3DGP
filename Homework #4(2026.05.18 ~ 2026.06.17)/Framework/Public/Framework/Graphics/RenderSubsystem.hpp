#pragma once

#include <map>
#include <memory>
#include <vector>

#include "../Core/EngineSubsystem.hpp"

#include "CommandBuffer.hpp"
#include "Fence.hpp"
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

		void CreateSwapChain(const Display& window);

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

	private:
		std::unique_ptr<GraphicsDevice> device;
		std::unique_ptr<CommandBuffer> commandBuffer;
		std::unique_ptr<Fence> fence;
		std::map<const Display*, std::unique_ptr<SwapChain>> swapChains;
	};
}
