#include "Precompiled.hpp"
#include "D3DFence.hpp"

#include "D3DDevice.hpp"

namespace TUK::Framework
{
	std::expected<void, std::string> D3DFence::Wait() const
	{
		const std::uint64_t completedValue = d3dFence->GetCompletedValue();
		if (completedValue == std::numeric_limits<std::uint64_t>::max())
		{
			return std::unexpected("GPU 장치가 제거되어 Fence 완료를 확인할 수 없습니다.");
		}

		if (completedValue < fenceValue)
		{
			// 이벤트 핸들이 nullptr이면 해당 값에 도달할 때까지 이 호출에서 대기합니다.
			if (FAILED(d3dFence->SetEventOnCompletion(fenceValue, nullptr)))
			{
				return std::unexpected("GPU Fence 대기에 실패했습니다.");
			}
		}

		if (!IsCompleted())
		{
			return std::unexpected("GPU Fence가 정상적으로 완료되지 않았습니다.");
		}

		return {};
	}

	std::expected<void, std::string> D3DFence::Signal(const D3DDevice& device)
	{
		if (fenceValue >= std::numeric_limits<std::uint64_t>::max() - 1)
		{
			return std::unexpected("GPU Fence 값의 범위를 초과했습니다.");
		}

		const std::uint64_t nextFenceValue = fenceValue + 1;
		if (FAILED(device.GetD3DCommandQueue().Signal(d3dFence.Get(), nextFenceValue)))
		{
			return std::unexpected("GPU Fence Signal에 실패했습니다.");
		}

		fenceValue = nextFenceValue;
		return {};
	}

	bool D3DFence::IsCompleted() const noexcept
	{
		const std::uint64_t completedValue = d3dFence->GetCompletedValue();
		return completedValue != std::numeric_limits<std::uint64_t>::max() && completedValue >= fenceValue;
	}

	D3DFence::D3DFence(Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence) noexcept
		: d3dFence(std::move(d3dFence))
		, fenceValue(0)
	{
	}
}