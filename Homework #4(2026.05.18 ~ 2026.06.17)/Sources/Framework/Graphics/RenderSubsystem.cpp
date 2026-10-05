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
		, factory()
		, device()
		, commandQueue()
		, renderer()
		, fence()
		, fenceEvent(nullptr)
		, fenceValue(0)
		, swapChains()
		, frameSwapChains()
		, isFrameRecording(false)
		, isInitialized(false)
		, hasFailed(false)
	{
	}

	RenderSubsystem::~RenderSubsystem() noexcept
	{
		if (fenceEvent)
		{
			CloseHandle(fenceEvent);
		}
	}

	ID3D12Device& RenderSubsystem::GetDevice() const noexcept
	{
		assert(device);
		return *device.Get();
	}

	Renderer& RenderSubsystem::GetRenderer() noexcept
	{
		return renderer;
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

	bool RenderSubsystem::InitializeDevice()
	{
		UINT factoryFlags = 0;
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debug;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()))))
		{
			debug->EnableDebugLayer();
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		if (!CheckResult(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(factory.GetAddressOf())), "DXGI 팩토리 생성"))
		{
			return false;
		}

		for (UINT index = 0; ; ++index)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
			const HRESULT result = factory->EnumAdapterByGpuPreference(index,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf()));
			if (result == DXGI_ERROR_NOT_FOUND)
			{
				break;
			}
			if (!CheckResult(result, "어댑터 조회"))
			{
				return false;
			}

			DXGI_ADAPTER_DESC1 description{};
			if (!CheckResult(adapter->GetDesc1(&description), "어댑터 정보 조회"))
			{
				return false;
			}
			if (description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(device.ReleaseAndGetAddressOf()))))
			{
				return true;
			}
		}

		// DX12를 지원하는 하드웨어가 없으면 소프트웨어 디바이스 사용.
		Microsoft::WRL::ComPtr<IDXGIAdapter> warp;
		if (!CheckResult(factory->EnumWarpAdapter(IID_PPV_ARGS(warp.GetAddressOf())), "WARP 어댑터 조회"))
		{
			return false;
		}
		return CheckResult(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(device.ReleaseAndGetAddressOf())), "D3D12 디바이스 생성");
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
		if (!InitializeDevice())
		{
			return;
		}

		D3D12_COMMAND_QUEUE_DESC queueDescription{};
		queueDescription.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		if (!CheckResult(device->CreateCommandQueue(&queueDescription,
			IID_PPV_ARGS(commandQueue.GetAddressOf())), "명령 큐 생성"))
		{
			return;
		}
		if (!CheckResult(renderer.Initialize(*device.Get())))
		{
			return;
		}
		if (!CheckResult(device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(fence.GetAddressOf())), "펜스 생성"))
		{
			return;
		}
		fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (!fenceEvent)
		{
			CheckResult(HRESULT_FROM_WIN32(GetLastError()), "펜스 이벤트 생성");
			return;
		}
		fenceValue = 0;
		isInitialized = true;
	}

	void RenderSubsystem::OnPreTick()
	{
		frameSwapChains.clear();
		if (!isInitialized || hasFailed)
		{
			return;
		}
		assert(windowSubsystem && device && commandQueue);
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
				swapChains.emplace_back(*window);
				iterator = std::prev(swapChains.end());
				if (!CheckResult(iterator->Initialize(*device.Get(), *factory.Get(), *commandQueue.Get(), width, height), "렌더 타깃 생성"))
				{
					return;
				}
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
		if (!CheckResult(renderer.Begin()))
		{
			return;
		}

		constexpr std::array<float, 4> backgroundColor = { 0.08f, 0.12f, 0.18f, 1.0f };
		for (const auto index : frameSwapChains)
		{
			if (!CheckResult(renderer.ClearSwapChain(swapChains[index], backgroundColor)))
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
		assert(windowSubsystem && device && commandQueue);
		for (const auto index : frameSwapChains)
		{
			if (!CheckResult(renderer.EndSwapChain(swapChains[index])))
			{
				return;
			}
		}
		isFrameRecording = false;
		if (!CheckResult(renderer.End()))
		{
			return;
		}
		if (!CheckResult(renderer.Execute(*commandQueue.Get())))
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
		assert(device && commandQueue && fence && fenceEvent);
		if (!CheckResult(device->GetDeviceRemovedReason(), "디바이스 상태 확인"))
		{
			return false;
		}
		const UINT64 value = ++fenceValue;
		if (!CheckResult(commandQueue->Signal(fence.Get(), value), "펜스 신호 전송"))
		{
			return false;
		}
		if (fence->GetCompletedValue() < value)
		{
			if (!CheckResult(fence->SetEventOnCompletion(value, fenceEvent), "펜스 이벤트 설정"))
			{
				return false;
			}
			if (WaitForSingleObject(fenceEvent, INFINITE) != WAIT_OBJECT_0)
			{
				return CheckResult(HRESULT_FROM_WIN32(GetLastError()), "GPU 작업 대기");
			}
		}
		return CheckResult(device->GetDeviceRemovedReason(), "GPU 작업 완료 확인");
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
		renderer.Shutdown();
		commandQueue.Reset();
		fence.Reset();
		if (fenceEvent)
		{
			CloseHandle(fenceEvent);
			fenceEvent = nullptr;
		}
		factory.Reset();
		device.Reset();
		windowSubsystem = nullptr;
		isInitialized = false;
	}
}
