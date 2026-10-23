#pragma once

#include <expected>
#include <string>

#include <d3d12.h>

#include <wrl/client.h>

#include "GPUCommandBuffer.hpp"
#include "GPUFence.hpp"

namespace TUK::Framework
{
	class GPUDevice
	{
		friend class RenderSubsystem;

	public:
		~GPUDevice() noexcept = default;

		GPUDevice(const GPUDevice&) = delete;
		GPUDevice& operator=(const GPUDevice&) = delete;

		GPUDevice(GPUDevice&&) noexcept = default;
		GPUDevice& operator=(GPUDevice&&) noexcept = default;

		[[nodiscard]] std::expected<GPUCommandBuffer, std::string> CreateGPUCommandBuffer() const;
		[[nodiscard]] std::expected<GPUFence, std::string> CreateGPUFence() const;

		[[nodiscard]] std::expected<void, std::string> SubmitCommandBuffer(GPUCommandBuffer& commandBuffer);

		[[nodiscard]] ID3D12Device& GetD3DDevice() const noexcept;
		[[nodiscard]] ID3D12CommandQueue& GetD3DCommandQueue() const noexcept;

	private:
		explicit GPUDevice(
			Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice,
			Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue) noexcept;

		Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue;
	};
}
