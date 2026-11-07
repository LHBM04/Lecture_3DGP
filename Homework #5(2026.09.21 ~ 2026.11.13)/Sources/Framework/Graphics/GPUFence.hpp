#pragma once

#include <expected>
#include <string>
#include <utility>

#include <wrl/client.h>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class GPUFence
	{
	public:

		GPUFence(const GPUFence&) = delete;
		GPUFence& operator=(const GPUFence&) = delete;

		GPUFence(GPUFence&&) noexcept = default;
		GPUFence& operator=(GPUFence&&) noexcept = default;

		void Wait() const;
		void Signal(const GPUDevice& device);

		[[nodiscard]] bool IsCompleted() const noexcept;

		[[nodiscard]] static std::expected<GPUFence, std::string> Create(const GPUDevice& device);

	private:
		explicit GPUFence(ComPtr<ID3D12Fence> d3dFence) noexcept;

		ComPtr<ID3D12Fence> d3dFence;
		u64 fenceValue;
	};

	inline GPUFence::GPUFence(ComPtr<ID3D12Fence> d3dFence) noexcept
		: d3dFence(std::move(d3dFence))
		, fenceValue(0)
	{
	}
}
