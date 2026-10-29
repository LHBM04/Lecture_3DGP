#pragma once

#include <expected>
#include <optional>
#include <string>

#include <d3d12.h>

#include <wrl/client.h>

#include "GPUBuffer.hpp"
#include "GPUCommandBuffer.hpp"
#include "GPUFence.hpp"
#include "GPUGraphicsPipelineState.hpp"
#include "GPUMesh.hpp"
#include "GPURootSignature.hpp"
#include "GPUShader.hpp"

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

		[[nodiscard]] std::expected<GPUBuffer, std::string> CreateGPUBuffer(const GPUBufferDescription& info) const;

		// 버퍼 전체를 메쉬 데이터로 사용합니다. 실패 시에도 전달된 버퍼의 소유권은 소비됩니다.
		[[nodiscard]] std::expected<GPUMesh, std::string> CreateGPUMesh(
			GPUBuffer vertexBuffer,
			const GPUMeshCreateInfo& info,
			std::optional<GPUBuffer> indexBuffer = std::nullopt) const;

		[[nodiscard]] std::expected<GPUShader, std::string> CreateGPUShader(const GPUShaderCreateInfo& info) const;
		[[nodiscard]] std::expected<GPURootSignature, std::string> CreateGPURootSignature(const GPURootSignatureCreateInfo& info) const;
		[[nodiscard]] std::expected<GPUGraphicsPipelineState, std::string> CreateGPUGraphicsPipelineState(const GPUGraphicsPipelineStateCreateInfo& info) const;

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
