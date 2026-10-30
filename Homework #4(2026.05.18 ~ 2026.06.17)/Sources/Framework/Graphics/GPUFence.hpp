#pragma once

#include <cstdint>
#include <expected>
#include <string>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	class GPUDevice;

	class GPUFence
	{
		friend class GPUDevice;

	public:
		~GPUFence() noexcept = default;

		GPUFence(const GPUFence&) = delete;
		GPUFence& operator=(const GPUFence&) = delete;

		GPUFence(GPUFence&&) noexcept = default;
		GPUFence& operator=(GPUFence&&) noexcept = default;

		[[nodiscard]] std::expected<void, std::string> Wait() const;
		[[nodiscard]] std::expected<void, std::string> Signal(const GPUDevice& device);

		[[nodiscard]] bool IsCompleted() const noexcept;

	private:
		explicit GPUFence(Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence) noexcept;

		Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence;
		std::uint64_t fenceValue;
	};
}
