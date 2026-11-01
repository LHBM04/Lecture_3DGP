#pragma once

#include <functional>
#include <memory_resource>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../Core/Common.hpp"
#include "../Core/EngineSubsystem.hpp"

#include "../Platform/Display.hpp"

#include "GPUCommandBuffer.hpp"
#include "GPUDevice.hpp"
#include "GPUFence.hpp"
#include "GPUSwapChain.hpp"

namespace TUK::Framework
{
	struct DisplayCreateEvent;
	struct DisplayCloseEvent;

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

		[[nodiscard]] GPUDevice& GetDevice() noexcept;
		[[nodiscard]] const GPUDevice& GetDevice() const noexcept;

	private:
		void AddListener(RenderCallback callback);

		std::optional<GPUDevice> device;
		std::optional<GPUCommandBuffer> commandBuffer;
		std::pmr::unordered_map<Display::ID, GPUSwapChain> swapChains;
		std::optional<GPUFence> fence;

		std::vector<RenderCallback> listeners;
	};

	inline RenderSubsystem::RenderSubsystem() noexcept
		: EngineSubsystem(20)
	{
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

	inline GPUDevice& RenderSubsystem::GetDevice() noexcept
	{
		return *device;
	}

	inline const GPUDevice& RenderSubsystem::GetDevice() const noexcept
	{
		return *device;
	}
}
