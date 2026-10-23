#pragma once

#include <expected>
#include <optional>

#include "../Core/EngineSubsystem.hpp"

#include "GPUDevice.hpp"
#include "GPUSwapChain.hpp"

namespace TUK::Framework
{
	class Display;
	class GPUCommandBuffer;
	class GPUFence;

	class RenderSubsystem : public EngineSubsystem
	{
	public:
		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override;

		void RegisterDisplay(const Display& display);
		void UnregisterDisplay();

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
		std::optional<GPUFence> fence;
		std::unordered_map<const Display*, GPUSwapChain> swapChains;
	};
}
