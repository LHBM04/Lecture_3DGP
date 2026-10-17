#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <d3d12.h>

#include "Framework/Graphics/Fence.hpp"

namespace TUK::Framework
{
	class Fence_Impl : public Fence
	{
	public:
		Fence_Impl(ID3D12Fence* d3dFence, HANDLE fenceEvent) noexcept;
		~Fence_Impl() noexcept override;

		void Wait() const override;
		void Signal(ID3D12CommandQueue& d3dCommandQueue);

		[[nodiscard]] bool IsCompleted() const noexcept override;

		/** D3D12 펜스 Getter */
		[[nodiscard]] ID3D12Fence& GetD3D12Fence() const noexcept;

	private:
		/** D3D12 펜스 */
		ID3D12Fence* d3dFence = nullptr;

		/** 펜스 이벤트 */
		HANDLE fenceEvent = nullptr;

		/** 펜스 값 */
		UINT64 fenceValue = 0;
	};
}
