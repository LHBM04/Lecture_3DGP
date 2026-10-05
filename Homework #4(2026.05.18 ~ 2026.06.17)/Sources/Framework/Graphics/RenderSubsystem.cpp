#include "Precompiled.h"
#include "RenderSubsystem.h"
#include "../Core/System.h"
#include "../Platform/WindowSubsystem.h"

#include <iterator>
#include <cassert>

namespace TUK::Framework
{
	RenderSubsystem::RenderSubsystem() noexcept
		: Subsystem(10)
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

	std::expected<std::reference_wrapper<SwapChain>, std::string> RenderSubsystem::GetSwapChain(Window& window)
	{
		if (!isFrameRecording || hasFailed)
		{
			return std::unexpected(std::string{ "현재 프레임에서 렌더링할 수 없습니다." });
		}
		for (const auto index : frameSwapChains)
		{
			if (swapChains[index].GetHWND() == window.GetHWND() && !window.ShouldClose())
			{
				return std::ref(swapChains[index]);
			}
		}
		return std::unexpected(std::string{ "해당 창의 활성 렌더 타깃이 없습니다." });
	}

	bool RenderSubsystem::CheckResult(const std::expected<void, std::string>& result)
	{
		if (result)
		{
			return true;
		}
		hasFailed = true;
		OutputDebugStringA(result.error().c_str());
		System::GetInstance().RequestQuit(EXIT_FAILURE);
		return false;
	}

	bool RenderSubsystem::CheckResult(HRESULT result, std::string_view operation)
	{
		if (SUCCEEDED(result))
		{
			return true;
		}

		hasFailed = true;
		const auto message = std::format("RenderSubsystem: {} 실패 (HRESULT: 0x{:08X}).\n",
			operation, static_cast<unsigned long>(result));
		OutputDebugStringA(message.c_str());
		System::GetInstance().RequestQuit(EXIT_FAILURE);
		return false;
	}

	void RenderSubsystem::OnStartup()
	{
		if (isInitialized)
		{
			return;
		}
		hasFailed = false;
		const auto windows = System::GetInstance().GetSubsystem<WindowSubsystem>();
		if (!windows)
		{
			OutputDebugStringA(windows.error().c_str());
			hasFailed = true;
			System::GetInstance().RequestQuit(EXIT_FAILURE);
			return;
		}
		windowSubsystem = &windows->get();
		if (!CheckResult(device.Initialize()))
		{
			return;
		}

		D3D12_COMMAND_QUEUE_DESC queueDescription{};
		queueDescription.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		const auto queue = device.CreateCommandQueue(queueDescription);
		if (!queue)
		{
			CheckResult(std::expected<void, std::string>{ std::unexpected(queue.error()) });
			return;
		}
		commandQueue = *queue;
		if (!CheckResult(renderContext.Initialize(device.GetNativeDevice())))
		{
			return;
		}
		auto createdFence = device.CreateFence(0, D3D12_FENCE_FLAG_NONE);
		if (!createdFence)
		{
			CheckResult(std::expected<void, std::string>{ std::unexpected(createdFence.error()) });
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
				if (!createdSwapChain)
				{
					CheckResult(std::expected<void, std::string>{ std::unexpected(createdSwapChain.error()) });
					return;
				}
				swapChains.push_back(std::move(*createdSwapChain));
				iterator = std::prev(swapChains.end());
			}
			else if (iterator->GetSizeX() != width || iterator->GetSizeY() != height)
			{
				if (!CheckResult(iterator->Resize(width, height), "렌더 타깃 크기 변경"))
				{
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

		constexpr std::array<float, 4> backgroundColor = { 0.08f, 0.12f, 0.18f, 1.0f };
		for (const auto index : frameSwapChains)
		{
			if (!CheckResult(renderContext.ClearSwapChain(swapChains[index], backgroundColor)))
			{
				return;
			}
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
			if (!CheckResult(renderContext.EndSwapChain(swapChains[index])))
			{
				return;
			}
		}
		isFrameRecording = false;
		if (!CheckResult(renderContext.End()))
		{
			return;
		}
		if (!CheckResult(renderContext.Execute(*commandQueue.Get())))
		{
			return;
		}

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
			if (!CheckResult(target.Present(), "프레임 표시"))
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
