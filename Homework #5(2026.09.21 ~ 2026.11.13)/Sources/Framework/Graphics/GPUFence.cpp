#include "Precompiled.hpp"
#include "GPUFence.hpp"

#include <limits>

#include "GPUDevice.hpp"

namespace TUK::Framework
{
	std::expected<GPUFence, std::string> GPUFence::Create(const GPUDevice& device)
	{
		ComPtr<ID3D12Fence> d3dFence;
		if (FAILED(device.GetD3DDevice().CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(d3dFence.GetAddressOf()))))
		{
			return std::unexpected("D3D12 펜스 생성에 실패했습니다.");
		}

		return GPUFence(std::move(d3dFence));
	}

	void GPUFence::Wait() const
	{
		const u64 completedValue = d3dFence->GetCompletedValue();
		if (completedValue < fenceValue)
		{
			d3dFence->SetEventOnCompletion(fenceValue, nullptr);
		}
	}

	void GPUFence::Signal(const GPUDevice& device)
	{
		const u64 nextFenceValue = fenceValue + 1;
		device.GetD3DCommandQueue().Signal(d3dFence.Get(), nextFenceValue);

		fenceValue = nextFenceValue;
	}

	bool GPUFence::IsCompleted() const noexcept
	{
		const u64 completedValue = d3dFence->GetCompletedValue();
		return completedValue != std::numeric_limits<u64>::max() && completedValue >= fenceValue;
	}
}
