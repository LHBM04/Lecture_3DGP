#pragma once

#include <expected>
#include <string>
#include <string_view>

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

		[[nodiscard]] std::expected<GPUBuffer, std::string> CreateGPUBuffer(const D3D12_HEAP_PROPERTIES& heapProperties, const D3D12_RESOURCE_DESC& description, D3D12_RESOURCE_STATES initialState) const;

		[[nodiscard]] std::expected<GPUMesh, std::string> CreateGPUMesh(
			GPUBuffer vertexBuffer,
			std::uint32_t vertextStride,
			GPUBuffer indexBuffer,
			DXGI_FORMAT indexFormat,
			D3D12_PRIMITIVE_TOPOLOGY topology) const;

		[[nodiscard]] std::expected<GPUShader, std::string> CreateGPUShader(const std::filesystem::path& path, std::string_view target, const std::string& entryPoint, std::uint32_t compileFlags) const;
		[[nodiscard]] std::expected<GPURootSignature, std::string> CreateGPURootSignature(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& description) const;
		[[nodiscard]] std::expected<GPUGraphicsPipelineState, std::string> CreateGPUGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const;

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
