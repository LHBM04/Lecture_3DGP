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

#include "D3DCommandBuffer.hpp"
#include "D3DDevice.hpp"
#include "D3DFence.hpp"
#include "D3DSwapChain.hpp"

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

		void Subscribe(std::function<void(D3DCommandBuffer&)> callback);

		template <class TInstance>
		void Subscribe(TInstance* instance, void (TInstance::*callback)(D3DCommandBuffer&));

		template <class TInstance>
		void Subscribe(const TInstance* instance, void (TInstance::*callback)(D3DCommandBuffer&) const);

		[[nodiscard]] const D3DDevice* GetD3DDevice() const noexcept;
		[[nodiscard]] const D3DSwapChain* GetD3DSwapChain(const Display& display) const noexcept;
		[[nodiscard]] std::expected<void, std::string> WaitForGPU();

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

	private:
		RenderSubsystem() noexcept;

		using RenderCallback = std::function<void(D3DCommandBuffer&)>;

		std::expected<D3DDevice, std::string> CreateD3DDevice() const;
		std::expected<D3DSwapChain, std::string> CreateD3DSwapChain(HWND window, const DXGI_SWAP_CHAIN_DESC1& description) const;
		
		std::optional<D3DDevice> device;
		std::optional<D3DCommandBuffer> commandBuffer;
		std::unordered_map<const Display*, D3DSwapChain> swapChains;
		std::optional<D3DFence> fence;

		std::vector<RenderCallback> listeners;
	};

	template <class TInstance>
	void RenderSubsystem::Subscribe(TInstance* instance, void (TInstance::*callback)(D3DCommandBuffer&))
	{
		assert(instance && callback);
		Subscribe(
			[instance, callback](D3DCommandBuffer& renderer)
			{
				(instance->*callback)(renderer);
			});
	}

	template <class TInstance>
	void RenderSubsystem::Subscribe(const TInstance* instance, void (TInstance::*callback)(D3DCommandBuffer&) const)
	{
		assert(instance && callback);
		Subscribe(
			[instance, callback](D3DCommandBuffer& renderer)
			{
				(instance->*callback)(renderer);
			});
	}
}
