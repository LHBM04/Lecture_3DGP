#pragma once

#include <cassert>
#include <expected>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

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
		friend class System;

	public:
		~RenderSubsystem() noexcept override;

		void RegisterDisplay(const Display& display);
		void UnregisterDisplay(const Display& display);

		void Subscribe(std::function<void(GPUCommandBuffer&)> callback);

		template <class TInstance>
		void Subscribe(TInstance* instance, void (TInstance::*callback)(GPUCommandBuffer&));

		template <class TInstance>
		void Subscribe(const TInstance* instance, void (TInstance::*callback)(GPUCommandBuffer&) const);

		[[nodiscard]] const GPUDevice* GetGPUDevice() const noexcept;
		[[nodiscard]] const GPUSwapChain* GetGPUSwapChain(const Display& display) const noexcept;
		[[nodiscard]] std::expected<void, std::string> WaitForGPU();

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

	private:
		using RenderCallback = std::function<void(GPUCommandBuffer&)>;

		RenderSubsystem() noexcept;

		std::expected<GPUDevice, std::string> CreateGPUDevice() const;
		std::expected<GPUSwapChain, std::string> CreateGPUSwapChain(HWND window, const DXGI_SWAP_CHAIN_DESC1& description) const;
		
		std::optional<GPUDevice> device;
		std::optional<GPUCommandBuffer> commandBuffer;
		std::unordered_map<const Display*, GPUSwapChain> swapChains;
		std::optional<GPUFence> fence;

		std::vector<RenderCallback> listeners;
	};

	template <class TInstance>
	void RenderSubsystem::Subscribe(TInstance* instance, void (TInstance::*callback)(GPUCommandBuffer&))
	{
		assert(instance && callback);
		Subscribe(
			[instance, callback](GPUCommandBuffer& commandBuffer)
			{
				(instance->*callback)(commandBuffer);
			});
	}

	template <class TInstance>
	void RenderSubsystem::Subscribe(const TInstance* instance, void (TInstance::*callback)(GPUCommandBuffer&) const)
	{
		assert(instance && callback);
		Subscribe(
			[instance, callback](GPUCommandBuffer& commandBuffer)
			{
				(instance->*callback)(commandBuffer);
			});
	}
}
