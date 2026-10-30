#pragma once

#include <cstdint>

#include <d3d12.h>

#include <wrl/client.h>

#include "../Math/ColorRGBA.hpp"

namespace TUK::Framework
{
	class GPUBuffer;
	class GPUGraphicsPipelineState;
	class GPUMesh;
	class GPURootSignature;
	class GPURenderTarget;
	class GPUTexture;

	class GPUCommandBuffer
	{
		friend class GPUDevice;

	public:
		~GPUCommandBuffer() noexcept = default;

		GPUCommandBuffer(const GPUCommandBuffer&) = delete;
		GPUCommandBuffer& operator=(const GPUCommandBuffer&) = delete;

		GPUCommandBuffer(GPUCommandBuffer&&) noexcept = default;
		GPUCommandBuffer& operator=(GPUCommandBuffer&&) noexcept = default;

		void TransitionTexture(
			const GPUTexture& texture,
			D3D12_RESOURCE_STATES stateBefore,
			D3D12_RESOURCE_STATES stateAfter);

		void TransitionBuffer(
			const GPUBuffer& buffer,
			D3D12_RESOURCE_STATES stateBefore,
			D3D12_RESOURCE_STATES stateAfter);

		void CopyGPUBuffer(
			const GPUBuffer& destination,
			std::uint64_t destinationOffset,
			const GPUBuffer& source,
			std::uint64_t sourceOffset,
			std::uint64_t sizeInBytes);

		void SetGPUGraphicsRootSignature(const GPURootSignature& rootSignature);
		void SetGPUGraphicsPipelineState(const GPUGraphicsPipelineState& pipelineState);

		void SetGPUGraphicsConstantBuffer(std::uint32_t rootParameterIndex, const GPUBuffer& buffer, std::uint64_t offset = 0);
		void SetViewport(const D3D12_VIEWPORT& viewport);
		void SetScissorRect(const D3D12_RECT& scissorRect);

		void SetGPUMesh(const GPUMesh& mesh);

		void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex = 0);
		void DrawIndexed(std::uint32_t indexCount, std::uint32_t firstIndex = 0, std::int32_t baseVertex = 0);

		void ClearGPURenderTarget(const GPURenderTarget& target, const ColorRGBA<float>& clearColor);
		void SetGPURenderTarget(const GPURenderTarget& target);

		[[nodiscard]] ID3D12GraphicsCommandList10& GetD3DCommandList() const noexcept;
		[[nodiscard]] ID3D12CommandAllocator& GetD3DCommandAllocator() const noexcept;

	private:
		explicit GPUCommandBuffer(
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList,
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator) noexcept;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
	};
}
