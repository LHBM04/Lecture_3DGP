#include "Precompiled.hpp"
#include "RenderSubsystem.hpp"

#include <iostream>
#include <ranges>
#include <utility>

#include "../Platform/Display.hpp"
#include "../Platform/DisplayEvents.hpp"

#include "../Utilities/Logger.hpp"

#include "GPUTexture2D.hpp"

namespace TUK::Framework
{
	SystemStatus RenderSubsystem::OnStartup()
	{
		auto newDevice = GPUDevice::Create();
		if (!newDevice)
		{
			LOG_CRITICAL("GPU 디바이스 생성에 실패했습니다: {}", newDevice.error());
			return SystemStatus::FAILURE;
		}
		device = std::move(*newDevice);

		auto newCommandBuffer = GPUCommandBuffer::Create(*device);
		if (!newCommandBuffer)
		{
			LOG_ERROR("GPU 명령 버퍼 생성에 실패했습니다: {}", newCommandBuffer.error());
			return SystemStatus::FAILURE;
		}
		commandBuffer = std::move(*newCommandBuffer);

		auto newFence = GPUFence::Create(*device);
		if (!newFence)
		{
			LOG_ERROR("GPU 펜스 생성에 실패했습니다: {}", newFence.error());
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
			LOG_ERROR("{}", result.error());
			return SystemStatus::FAILURE;
		}

		for (auto& [display, swapChain] : swapChains)
		{
			const GPUTarget target = swapChain.GetCurrentBackBufferTarget();
			const GPUTexture2D& backBuffer = target.GetTexture();

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
			LOG_CRITICAL("GPU 명령 버퍼 제출에 실패했습니다: {}", result.error());
			return SystemStatus::FAILURE;
		}

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

	void RenderSubsystem::OnDisplayCreate(const DisplayCreateEvent& event)
	{
		const Display* display = event.display;
		if (!display)
		{
			return;
		}

		DXGI_SWAP_CHAIN_DESC1 description{};
		description.Width = display->GetClientSize().GetX();
		description.Height = display->GetClientSize().GetY();
		description.BufferCount = 2;
		description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		description.SampleDesc.Count = 1;

		auto result = GPUSwapChain::Create(*device, display, description);
		if (!result)
		{
			LOG_ERROR("GPU 스왑체인 생성에 실패했습니다: {}", result.error());
			return;
		}

		swapChains.emplace(display->GetID(), std::move(*result));
	}

	void RenderSubsystem::OnDisplayClose(const DisplayCloseEvent& event)
	{
		const Display* display = event.display;
		if (!display)
		{
			return;
		}

		auto result = swapChains.find(display->GetID());
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
