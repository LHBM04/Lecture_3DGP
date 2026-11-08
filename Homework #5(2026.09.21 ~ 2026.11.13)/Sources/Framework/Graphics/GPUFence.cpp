#include "Precompiled.hpp"
#include "GPUFence.hpp"

#include <limits>

#include "GPUDevice.hpp"

namespace TUK::Framework
{
	void GPUFence::Wait() const
	{
		ASSERT_FMT(d3dFence.Get(), "d3dFence가 nullptr입니다.");

		const u64 completedValue = d3dFence->GetCompletedValue();
		if (completedValue < fenceValue)
		{
			d3dFence->SetEventOnCompletion(fenceValue, nullptr);
		}
	}

	void GPUFence::Signal(const GPUDevice& device)
	{
		ASSERT_FMT(d3dFence.Get(), "d3dFence가 nullptr입니다.");

		const u64 nextFenceValue = fenceValue + 1;
		device.GetD3DCommandQueue().Signal(d3dFence.Get(), nextFenceValue);

		fenceValue = nextFenceValue;
	}

	bool GPUFence::IsCompleted() const noexcept
	{
		ASSERT_FMT(d3dFence.Get(), "d3dFence가 nullptr입니다.");

		const u64 completedValue = d3dFence->GetCompletedValue();
		return completedValue != std::numeric_limits<u64>::max() && completedValue >= fenceValue;
	}
}
