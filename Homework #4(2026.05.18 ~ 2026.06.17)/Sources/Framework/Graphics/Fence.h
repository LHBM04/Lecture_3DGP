#pragma once

#include <d3d12.h>
#include <expected>
#include <string>
#include <wrl.h>

namespace TUK::Framework
{
	class GraphicsDevice;

	/** 한 명령 큐에서 순차적으로 사용하며, 여러 스레드에서 동시에 호출하지 않는다. */
	class Fence
	{
		friend class GraphicsDevice;

	public:
		Fence() noexcept;
		~Fence() noexcept;

		Fence(const Fence&) = delete;
		Fence& operator=(const Fence&) = delete;
		Fence(Fence&& other) noexcept;
		Fence& operator=(Fence&& other) noexcept;

		/** 큐의 앞선 작업이 완료되면 도달할 값을 기록한다. */
		[[nodiscard]] std::expected<void, std::string> Signal(ID3D12CommandQueue& queue);
		/** 마지막으로 Signal한 작업의 완료 여부를 조회한다. */
		[[nodiscard]] std::expected<bool, std::string> IsComplete() const;
		/** 마지막으로 Signal한 작업이 완료될 때까지 CPU에서 대기한다. */
		[[nodiscard]] std::expected<void, std::string> Wait();
		/** 이 객체를 사용하는 작업 및 대기가 끝난 뒤 호출한다. */
		void Release() noexcept;

	private:
		[[nodiscard]] std::expected<void, std::string> Initialize(
			ID3D12Device& device, UINT64 initialValue, D3D12_FENCE_FLAGS flags);
		[[nodiscard]] std::expected<void, std::string> CheckDeviceStatus() const;

		Microsoft::WRL::ComPtr<ID3D12Device> device;
		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		HANDLE completionEvent;
		UINT64 value;
	};
}
