#pragma once

#include <utility>

#include <wrl/client.h>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class GPUFence
	{
	public:
		explicit GPUFence(Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence) noexcept;

		GPUFence(const GPUFence&) = delete;
		GPUFence& operator=(const GPUFence&) = delete;

		GPUFence(GPUFence&&) noexcept = default;
		GPUFence& operator=(GPUFence&&) noexcept = default;

		void Wait() const;
		void Signal(const GPUDevice& device);

		[[nodiscard]] bool IsCompleted() const noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence;
		u64 fenceValue;
	};

	inline GPUFence::GPUFence(Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence) noexcept
		: d3dFence(std::move(d3dFence))
		, fenceValue(0)
	{
		ASSERT_FMT(this->d3dFence.Get(), "d3dFence가 nullptr입니다.");
	}
}
