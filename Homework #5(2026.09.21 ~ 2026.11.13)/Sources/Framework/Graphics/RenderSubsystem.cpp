#include "Precompiled.hpp"
#include "RenderSubsystem.hpp"

#include <iostream>
#include <ranges>
#include <utility>

#include "../Platform/Display.hpp"
#include "../Platform/DisplayEvents.hpp"

#include "GPUTexture.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	SystemStatus RenderSubsystem::OnStartup()
	{
		auto newDevice = GPUDevice::Create();
		if (!newDevice)
		{
			Logger::LogError("{}{}", "RenderSubsystem startup failed: ", newDevice.error());
			return SystemStatus::FAILURE;
		}
		device = std::move(*newDevice);

		auto newCommandBuffer = GPUCommandBuffer::Create(*device);
		if (!newCommandBuffer)
		{
			Logger::LogError("{}{}", "RenderSubsystem startup failed: ", newCommandBuffer.error());
			return SystemStatus::FAILURE;
		}
		commandBuffer = std::move(*newCommandBuffer);

		auto newFence = GPUFence::Create(*device);
		if (!newFence)
		{
			Logger::LogError("{}{}", "RenderSubsystem startup failed: ", newFence.error());
			return SystemStatus::FAILURE;
		}
		fence = std::move(*newFence);

		return SystemStatus::CONTINUE;
	}

	SystemStatus RenderSubsystem::OnPreTick()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus RenderSubsystem::OnTick()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus RenderSubsystem::OnPostTick()
	{
		if (auto result = commandBuffer->Reset(); !result)
		{
			Logger::LogError("{}", result.error());
			return SystemStatus::FAILURE;
		}

		// 媛??ㅼ솑 泥댁씤???뚮㈃??紐낅졊??湲곕줉.
		for (auto& [display, swapChain] : swapChains)
		{
			const GPUTarget target = swapChain.GetCurrentBackBufferTarget();
			const GPUTexture& backBuffer = target.GetTexture();
			commandBuffer->TransitionTexture(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			commandBuffer->ClearGPUTarget(target, ColorRGBA<f32>::GetBlack());
			commandBuffer->SetGPUTarget(target);

			for (const RenderCallback& listener : listeners)
			{
				listener(*commandBuffer);
			}

			commandBuffer->TransitionTexture(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		}

		if (auto result = commandBuffer->Submit(); !result)
		{
			Logger::LogError("{}", result.error());
			return SystemStatus::FAILURE;
		}

		// 紐⑤뱺 ?ㅼ솑 泥댁씤??Present.
		for (auto& swapChain : swapChains | std::views::values)
		{
			swapChain.Present();
		}

		fence->Signal(*device);
		fence->Wait();

		return SystemStatus::CONTINUE;
	}

	void RenderSubsystem::OnShutdown()
	{
		listeners.clear();
		swapChains.clear();
		commandBuffer.reset();
		fence.reset();
		device.reset();
	}

	void RenderSubsystem::OnDisplayCreated(const DisplayCreateEvent& event)
	{
		const Display& display = event.display;

		DXGI_SWAP_CHAIN_DESC1 description{};
		description.Width = display.GetClientSize().GetX();
		description.Height = display.GetClientSize().GetY();
		description.BufferCount = 2;
		description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		description.SampleDesc.Count = 1;

		auto result = GPUSwapChain::Create(*device, display, description);
		if (!result)
		{
			Logger::LogError("{}{}", "RenderSubsystem display registration failed: ", result.error());
			return;
		}

		swapChains.emplace(display.GetID(), std::move(*result));
	}

	void RenderSubsystem::OnDisplayClosed(const DisplayCloseEvent& event)
	{
		const Display& display = event.display;

		auto result = swapChains.find(display.GetID());
		if (result != swapChains.end())
		{
			swapChains.erase(result);
		}
	}

	void RenderSubsystem::AddListener(RenderCallback callback)
	{
		listeners.push_back(std::move(callback));
	}
}
