#include "Precompiled.h"
#include "Fence.h"

#include "GraphicsError.h"

namespace TUK::Framework
{
	Fence::Fence() noexcept
		: device()
		, fence()
		, completionEvent(nullptr)
		, value(0)
	{
	}

	Fence::~Fence() noexcept
	{
		Release();
	}

	Fence::Fence(Fence&& other) noexcept
		: device(std::move(other.device))
		, fence(std::move(other.fence))
		, completionEvent(std::exchange(other.completionEvent, nullptr))
		, value(std::exchange(other.value, 0))
	{
	}

	Fence& Fence::operator=(Fence&& other) noexcept
	{
		if (this != &other)
		{
			Release();
			device = std::move(other.device);
			fence = std::move(other.fence);
			completionEvent = std::exchange(other.completionEvent, nullptr);
			value = std::exchange(other.value, 0);
		}
		return *this;
	}

	std::expected<void, std::string> Fence::Initialize(
		ID3D12Device& renderDevice, UINT64 initialValue, D3D12_FENCE_FLAGS flags)
	{
		if (fence || completionEvent)
		{
			return std::unexpected(std::string{ "Fence가 이미 초기화되어 있습니다." });
		}
		// UINT64_MAX는 디바이스 제거를 나타내는 완료 값이다.
		if (initialValue == (std::numeric_limits<UINT64>::max)())
		{
			return std::unexpected(std::string{ "Fence의 초기 값으로 UINT64_MAX를 사용할 수 없습니다." });
		}
		Microsoft::WRL::ComPtr<ID3D12Fence> createdFence;
		const auto result = CheckHResult(renderDevice.CreateFence(initialValue, flags,
			IID_PPV_ARGS(createdFence.GetAddressOf())), "펜스 생성");
		if (!result)
		{
			return result;
		}
		const HANDLE createdEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (!createdEvent)
		{
			return CheckHResult(HRESULT_FROM_WIN32(GetLastError()), "완료 이벤트 생성");
		}
		device = &renderDevice;
		fence = std::move(createdFence);
		completionEvent = createdEvent;
		value = initialValue;
		return {};
	}

	std::expected<void, std::string> Fence::CheckDeviceStatus() const
	{
		assert(device);
		return CheckHResult(device->GetDeviceRemovedReason(), "디바이스 상태 확인");
	}

	std::expected<void, std::string> Fence::Signal(ID3D12CommandQueue& queue)
	{
		assert(fence);
		const auto status = CheckDeviceStatus();
		if (!status)
		{
			return status;
		}
		if (value >= (std::numeric_limits<UINT64>::max)() - 1)
		{
			return std::unexpected(std::string{ "Fence 신호 값을 더 증가시킬 수 없습니다." });
		}
		const UINT64 nextValue = value + 1;
		const auto result = CheckHResult(queue.Signal(fence.Get(), nextValue), "펜스 신호 전송");
		if (!result)
		{
			return result;
		}
		value = nextValue;
		return {};
	}

	std::expected<bool, std::string> Fence::IsComplete() const
	{
		assert(fence);
		const auto status = CheckDeviceStatus();
		if (!status)
		{
			return std::unexpected(status.error());
		}
		const UINT64 completedValue = fence->GetCompletedValue();
		if (completedValue == (std::numeric_limits<UINT64>::max)())
		{
			return std::unexpected(std::string{ "Fence 완료 조회 중 디바이스가 제거되었습니다." });
		}
		return completedValue >= value;
	}

	std::expected<void, std::string> Fence::Wait()
	{
		assert(fence && completionEvent);
		const auto completed = IsComplete();
		if (!completed)
		{
			return std::unexpected(completed.error());
		}
		if (*completed)
		{
			return {};
		}
		const auto result = CheckHResult(fence->SetEventOnCompletion(value, completionEvent), "완료 이벤트 설정");
		if (!result)
		{
			return result;
		}
		const DWORD waitResult = WaitForSingleObject(completionEvent, INFINITE);
		if (waitResult == WAIT_FAILED)
		{
			return CheckHResult(HRESULT_FROM_WIN32(GetLastError()), "GPU 작업 대기");
		}
		if (waitResult != WAIT_OBJECT_0)
		{
			return std::unexpected(std::format("Fence: 예상하지 못한 대기 결과({}).", waitResult));
		}
		return CheckDeviceStatus();
	}

	void Fence::Release() noexcept
	{
		if (completionEvent)
		{
			CloseHandle(completionEvent);
			completionEvent = nullptr;
		}
		fence.Reset();
		device.Reset();
		value = 0;
	}
}
