#pragma once

#include <expected>
#include <functional>
#include <optional>
#include <string>
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

		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

		template <class TListener>
		void AddListener(TListener* listener, void (TListener::* callback)(GPUCommandBuffer&));

		template <class TListener>
		void AddListener(const TListener* listener, void (TListener::* callback)(GPUCommandBuffer&) const);

		void OnDisplayCreate(const DisplayCreateEvent& event);
		void OnDisplayClose(const DisplayCloseEvent& event);

		[[nodiscard]] GPUDevice& GetDevice() noexcept;
		[[nodiscard]] const GPUDevice& GetDevice() const noexcept;

	private:
		[[nodiscard]] std::expected<GPUDevice, std::string> CreateDevice() const;
		void AddListener(RenderCallback callback);

		std::optional<GPUDevice> device;
		std::optional<GPUCommandBuffer> commandBuffer;
		std::unordered_map<Display::ID, GPUSwapChain> swapChains;
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
		ASSERT_FMT(listener, "렌더 이벤트 수신 객체가 nullptr입니다.");
		ASSERT_FMT(callback, "렌더 이벤트 콜백이 nullptr입니다.");

		AddListener(
			[listener, callback](GPUCommandBuffer& commandBuffer)
			{
				(listener->*callback)(commandBuffer);
			});
	}

	template <class TInstance>
	void RenderSubsystem::AddListener(const TInstance* listener, void (TInstance::* callback)(GPUCommandBuffer&) const)
	{
		ASSERT_FMT(listener, "렌더 이벤트 수신 객체가 nullptr입니다.");
		ASSERT_FMT(callback, "렌더 이벤트 콜백이 nullptr입니다.");

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
