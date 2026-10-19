#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <d3d12.h>

namespace TUK::Framework
{
	class Fence
	{
	public:
		Fence(ID3D12Fence* d3dFence, HANDLE fenceEvent) noexcept;
		~Fence() noexcept;

		Fence(const Fence&) = delete;
		Fence& operator=(const Fence&) = delete;

		Fence(Fence&&) = delete;
		Fence& operator=(Fence&&) = delete;

		void Wait() const;
		void Signal(ID3D12CommandQueue& d3dCommandQueue);

		[[nodiscard]] bool IsCompleted() const noexcept;

		/** D3D12 ?쒖뒪 Getter */
		[[nodiscard]] ID3D12Fence& GetD3D12Fence() const noexcept;

	private:
		/** D3D12 ?쒖뒪 */
		ID3D12Fence* d3dFence = nullptr;

		/** ?쒖뒪 ?대깽??*/
		HANDLE fenceEvent = nullptr;

		/** ?쒖뒪 媛?*/
		UINT64 fenceValue = 0;
	};
}
