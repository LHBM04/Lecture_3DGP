#pragma once

#include <expected>
#include <optional>
#include <unordered_map>

#include "../Core/EngineSubsystem.hpp"

#include "GPUCommandBuffer.hpp"
#include "GPUDevice.hpp"
#include "GPUFence.hpp"
#include "GPUSwapChain.hpp"

namespace TUK::Framework
{
	class Display;

	class RenderSubsystem : public EngineSubsystem
	{
	public:
		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override;

		void RegisterDisplay(const Display& display);
		void UnregisterDisplay(const Display& display);

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

	private:
		std::expected<GPUDevice, std::string> CreateGPUDevice();
		std::expected<GPUSwapChain, std::string> CreateGPUSwapChain(const GPUSwapChainProperties& properties);
		
		std::optional<GPUDevice> device;
		std::optional<GPUCommandBuffer> commandBuffer;
		std::unordered_map<const Display*, GPUSwapChain> swapChains;
		std::optional<GPUFence> fence;
	};
}
