#pragma once

#include <expected>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

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
		RenderSubsystem() noexcept;

		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

		[[nodiscard]] Delegate<GPUCommandBuffer&>& OnRender() noexcept;

		void OnDisplayCreate(const DisplayCreateEvent& event);
		void OnDisplayClose(const DisplayCloseEvent& event);

		[[nodiscard]] GPUDevice& GetDevice() noexcept;
		[[nodiscard]] const GPUDevice& GetDevice() const noexcept;

	private:
		[[nodiscard]] std::expected<GPUDevice, std::string> CreateDevice() const;

		std::optional<GPUDevice> device;
		std::optional<GPUCommandBuffer> commandBuffer;
		std::unordered_map<Display::ID, GPUSwapChain> swapChains;
		std::optional<GPUFence> fence;

		Delegate<GPUCommandBuffer&> onRender;
	};

	inline RenderSubsystem::RenderSubsystem() noexcept
		: EngineSubsystem(20)
	{
	}

	inline Delegate<GPUCommandBuffer&>& RenderSubsystem::OnRender() noexcept
	{
		return onRender;
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
