#include "Precompiled.hpp"
#include "Fence.hpp"

namespace TUK::Framework
{
	Fence::Fence(ID3D12Fence* d3dFence, HANDLE fenceEvent) noexcept
		: d3dFence(d3dFence)
		, fenceEvent(fenceEvent)
		, fenceValue(0)
	{
		assert(d3dFence && fenceEvent);
	}

	Fence::~Fence() noexcept
	{
		if (fenceEvent)
		{
			CloseHandle(fenceEvent);
			fenceEvent = nullptr;
		}

		if (d3dFence)
		{
			d3dFence->Release();
			d3dFence = nullptr;
		}

		fenceValue = 0;
	}

	void Fence::Wait() const
	{
		assert(d3dFence && fenceEvent);

		if (!IsCompleted())
		{
			if (SUCCEEDED(d3dFence->SetEventOnCompletion(fenceValue, fenceEvent)))
			{
				WaitForSingleObject(fenceEvent, INFINITE);
			}
		}
	}

	bool Fence::IsCompleted() const noexcept
	{
		assert(d3dFence);
		return d3dFence->GetCompletedValue() >= fenceValue;
	}

	void Fence::Signal(ID3D12CommandQueue& d3dCommandQueue)
	{
		assert(d3dFence);

		const UINT64 nextFenceValue = fenceValue + 1;
		if (SUCCEEDED(d3dCommandQueue.Signal(d3dFence, nextFenceValue)))
		{
			fenceValue = nextFenceValue;
		}
	}

	ID3D12Fence& Fence::GetD3D12Fence() const noexcept
	{
		assert(d3dFence);
		return *d3dFence;
	}
}
