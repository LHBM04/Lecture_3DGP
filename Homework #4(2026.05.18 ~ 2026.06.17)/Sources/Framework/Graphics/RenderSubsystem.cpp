#include "Precompiled.h"
#include "RenderSubsystem.h"

#include "../Core/Engine.h"
#include "../Platform/WindowSubsystem.h"
#include "GraphicsError.h"

namespace TUK::Framework
{
	RenderSubsystem::RenderSubsystem() noexcept
		: EngineSubsystem(10)
		, windowSubsystem(nullptr)
		, device()
		, commandQueue()
		, renderContext()
		, fence()
		, swapChains()
		, frameSwapChains()
		, isFrameRecording(false)
		, isInitialized(false)
		, hasFailed(false)
	{
	}

	RenderSubsystem::~RenderSubsystem() noexcept = default;

	GraphicsDevice& RenderSubsystem::GetDevice() noexcept
	{
		return device;
	}

	RenderContext& RenderSubsystem::GetRenderContext() noexcept
	{
		return renderContext;
	}

	SwapChain& RenderSubsystem::GetSwapChain(Window& window)
	{
		assert(isFrameRecording && !hasFailed && !window.ShouldClose());
		const auto iterator = std::ranges::find_if(frameSwapChains, [this, &window](std::size_t index)
		{
			return swapChains[index].GetHWND() == window.GetHWND();
		});
		assert(iterator != frameSwapChains.end() && "The window must have an active swap chain.");
		return swapChains[*iterator];
	}

	void RenderSubsystem::ReportError(std::string_view message)
	{
		hasFailed = true;
		Engine::GetInstance().ReportError(message);
	}

	bool RenderSubsystem::CheckResult(HRESULT result, std::string_view operation)
	{
		return CheckResult(CheckHResult(result, operation));
	}

	void RenderSubsystem::OnStartup()
	{
		if (isInitialized)
		{
			return;
		}
		hasFailed = false;
		windowSubsystem = Engine::GetInstance().GetSubsystem<WindowSubsystem>();
		if (!CheckResult(device.Initialize()))
		{
			return;
		}

		D3D12_COMMAND_QUEUE_DESC queueDescription{};
		queueDescription.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		const auto queue = device.CreateCommandQueue(queueDescription);
		if (!CheckResult(queue))
		{
			return;
		}
		commandQueue = *queue;
		if (!CheckResult(renderContext.Initialize(device.GetNativeDevice())))
		{
			return;
		}
		auto createdFence = device.CreateFence(0, D3D12_FENCE_FLAG_NONE);
		if (!CheckResult(createdFence))
		{
			return;
		}
		fence = std::move(*createdFence);
		isInitialized = true;
	}

	void RenderSubsystem::OnPreTick()
	{
		frameSwapChains.clear();
		if (!isInitialized || hasFailed)
		{
			return;
		}
		assert(windowSubsystem && commandQueue);
		const auto& windows = windowSubsystem->GetWindows();
		std::erase_if(swapChains, [&windows](const auto& target)
		{
			return std::ranges::none_of(windows, [&target](const auto& window)
			{
				return !window->ShouldClose() && window->GetHWND() == target.GetHWND();
			});
		});

		for (const auto& window : windows)
		{
			const HWND handle = window->GetHWND();
			if (window->ShouldClose() || !handle || IsIconic(handle) || !IsWindowVisible(handle))
			{
				continue;
			}
			RECT client{};
			if (!GetClientRect(handle, &client))
			{
				CheckResult(HRESULT_FROM_WIN32(GetLastError()), "창 클라이언트 영역 조회");
				return;
			}
			const auto width = static_cast<UINT>(client.right - client.left);
			const auto height = static_cast<UINT>(client.bottom - client.top);
			if (width == 0 || height == 0)
			{
				continue;
			}

			auto iterator = std::ranges::find(swapChains, handle, &SwapChain::GetHWND);
			if (iterator == swapChains.end())
			{
				auto createdSwapChain = device.CreateSwapChain(*window, *commandQueue.Get(), width, height);
				if (!CheckResult(createdSwapChain))
				{
					return;
				}
				swapChains.push_back(std::move(*createdSwapChain));
				iterator = std::prev(swapChains.end());
			}
			else if (iterator->GetSizeX() != width || iterator->GetSizeY() != height)
			{
				iterator->Resize(width, height);
				if (!Engine::GetInstance().IsRunning())
				{
					hasFailed = true;
					return;
				}
			}
			frameSwapChains.push_back(static_cast<std::size_t>(iterator - swapChains.begin()));
		}
		if (frameSwapChains.empty())
		{
			return;
		}
		if (!CheckResult(renderContext.Begin()))
		{
			return;
		}

		constexpr std::array<float, 4> backgroundColor{ 0.08f, 0.12f, 0.18f, 1.0f };
		for (const auto index : frameSwapChains)
		{
			renderContext.ClearSwapChain(swapChains[index], backgroundColor);
		}
		isFrameRecording = true;
	}

	void RenderSubsystem::OnPostTick()
	{
		if (!isInitialized || !isFrameRecording)
		{
			return;
		}
		assert(windowSubsystem && commandQueue);
		for (const auto index : frameSwapChains)
		{
			renderContext.EndSwapChain(swapChains[index]);
		}
		isFrameRecording = false;
		if (!CheckResult(renderContext.End()))
		{
			return;
		}
		renderContext.Execute(*commandQueue.Get());

		// WindowSubsystem의 OnPostTick에서 이미 삭제된 창에는 Present하지 않는다.
		const auto& windows = windowSubsystem->GetWindows();
		for (const auto index : frameSwapChains)
		{
			auto& target = swapChains[index];
			const bool canPresent = std::ranges::any_of(windows, [&target](const auto& window)
			{
				return !window->ShouldClose() && window->GetHWND() == target.GetHWND();
			});
			if (!canPresent)
			{
				continue;
			}
			if (!CheckResult(target.Present()))
			{
				break;
			}
		}
		// Present 실패 시에도 제출한 작업을 정리한 뒤 종료한다.
		WaitForGpu();
		frameSwapChains.clear();
	}

	bool RenderSubsystem::WaitForGpu()
	{
		assert(commandQueue);
		if (!CheckResult(fence.Signal(*commandQueue.Get())))
		{
			return false;
		}
		return CheckResult(fence.Wait());
	}

	void RenderSubsystem::OnShutdown()
	{
		if (isInitialized && !hasFailed)
		{
			WaitForGpu();
		}
		swapChains.clear();
		frameSwapChains.clear();
		isFrameRecording = false;
		renderContext.Release();
		commandQueue.Reset();
		fence.Release();
		device.Release();
		windowSubsystem = nullptr;
		isInitialized = false;
	}
}
