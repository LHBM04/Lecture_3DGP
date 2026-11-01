#pragma once

#include <cstdint>
#include <expected>
#include <string>

#include <directx/d3d12.h>

#include <wrl/client.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class GPUFence
	{
	public:
		explicit GPUFence(ComPtr<ID3D12Fence> d3dFence) noexcept;
		~GPUFence() noexcept = default;

		GPUFence(const GPUFence&) = delete;
		GPUFence& operator=(const GPUFence&) = delete;

		GPUFence(GPUFence&&) noexcept = default;
		GPUFence& operator=(GPUFence&&) noexcept = default;

		void Wait() const;
		void Signal(const GPUDevice& device);

		[[nodiscard]] bool IsCompleted() const noexcept;

	private:
		ComPtr<ID3D12Fence> d3dFence;
		u64 fenceValue;
	};

	inline GPUFence::GPUFence(ComPtr<ID3D12Fence> d3dFence) noexcept
		: d3dFence(std::move(d3dFence))
		, fenceValue(0)
	{
	}
}
