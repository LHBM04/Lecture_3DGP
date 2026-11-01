#pragma once

#include <optional>

#include <wrl.h>

#include <dxgi1_6.h>

#include "../Core/Common.hpp"
#include "../Core/EngineSubsystem.hpp"

#include "GPUCommandBuffer.hpp"
#include "GPUDevice.hpp"
#include "GPUFence.hpp"
#include "GPUSwapChain.hpp"

namespace TUK::Framework
{
	struct DisplayCreateEvent;
	struct DisplayCloseEvent;

	class Display;

	class RenderSubsystem : public EngineSubsystem
	{
	public:
		using RenderCallback = std::function<void(GPUCommandBuffer&)>;

		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override = default;

		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

		template <class TListener>
		void AddListener(TListener* listener, void (TListener::* callback)(GPUCommandBuffer&));

		template <class TListener>
		void AddListener(const TListener* listener, void (TListener::* callback)(GPUCommandBuffer&) const);

		void OnDisplayCreated(const DisplayCreateEvent& event);
		void OnDisplayClosed(const DisplayCloseEvent& event);

		[[nodiscard]] GPUDevice& GetDevice();
		[[nodiscard]] const GPUDevice& GetDevice() const;

	private:
		void AddListener(RenderCallback callback);

		std::expected<GPUDevice, std::string> CreateGPUDevice();
		std::expected<GPUSwapChain, std::string> CreateGPUSwapChain(const Display& display, const DXGI_SWAP_CHAIN_DESC1& description);

		ComPtr<IDXGIFactory6> dxgiFactory;

		std::optional<GPUDevice> device;
		std::optional<GPUCommandBuffer> commandBuffer;
		std::pmr::unordered_map<u32, GPUSwapChain> swapChains;
		std::optional<GPUFence> fence;

		std::vector<RenderCallback> listeners;
	};

	inline RenderSubsystem::RenderSubsystem() noexcept
		: EngineSubsystem(20)
	{
	}

	inline GPUDevice& RenderSubsystem::GetDevice()
	{
		return device.value();
	}

	inline const GPUDevice& RenderSubsystem::GetDevice() const
	{
		return device.value();
	}

	template <class TInstance>
	void RenderSubsystem::AddListener(TInstance* listener, void (TInstance::* callback)(GPUCommandBuffer&))
	{
		AddListener(
			[listener, callback](GPUCommandBuffer& commandBuffer)
			{
				(listener->*callback)(commandBuffer);
			});
	}

	template <class TInstance>
	void RenderSubsystem::AddListener(const TInstance* listener, void (TInstance::* callback)(GPUCommandBuffer&) const)
	{
		AddListener(
			[listener, callback](GPUCommandBuffer& commandBuffer)
			{
				(listener->*callback)(commandBuffer);
			});
	}
}
