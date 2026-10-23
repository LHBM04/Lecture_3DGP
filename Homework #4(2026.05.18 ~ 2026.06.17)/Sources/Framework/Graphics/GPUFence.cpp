#include "Precompiled.hpp"
#include "GPUFence.hpp"

#include "GPUDevice.hpp"

namespace TUK::Framework
{
	void GPUFence::Wait() const
	{
		if (!IsCompleted())
		{
			if (SUCCEEDED(d3dFence->SetEventOnCompletion(fenceValue, fenceEvent)))
			{
				WaitForSingleObject(fenceEvent, INFINITE);
			}
		}
	}

	void GPUFence::Signal(const GPUDevice& device)
	{
		const UINT64 nextFenceValue = fenceValue + 1;
		if (SUCCEEDED(device.GetD3DCommandQueue().Signal(d3dFence.Get(), nextFenceValue)))
		{
			fenceValue = nextFenceValue;
		}
	}

	bool GPUFence::IsCompleted() const noexcept
	{
		return d3dFence->GetCompletedValue() >= fenceValue;
	}

	GPUFence::GPUFence(ID3D12Fence* d3dFence) noexcept
		: d3dFence(d3dFence)
		, fenceEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr))
		, fenceValue(0)
	{
	}
}
