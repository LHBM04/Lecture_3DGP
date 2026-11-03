#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <d3d12.h>

#include <wrl/client.h>

#include "GPUBuffer.hpp"
#include "GPUGraphicsPipelineState.hpp"

#include "../Math/ColorRGBA.hpp"
#include "../Math/Matrix4x4.hpp"

namespace TUK::Framework
{
	class Camera;
	class GPUDevice;
	class MaterialAsset;
	class GPUMesh;
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

		// 이전 제출의 GPU 작업이 끝난 뒤 호출합니다.
		[[nodiscard]] std::expected<void, std::string> Reset();
		[[nodiscard]] std::expected<void, std::string> SetCamera(const Camera& camera) noexcept;
		[[nodiscard]] std::expected<void, std::string> SetMaterial(const MaterialAsset& material) noexcept;
		[[nodiscard]] std::expected<void, std::string> SetObjectTransform(const Matrix4x4<float>& worldMatrix) noexcept;

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

		void SetGPUGraphicsPipelineState(const GPUGraphicsPipelineState& pipelineState);

		void SetGPUCameraConstantBuffer(const GPUBuffer& buffer, std::uint64_t offset = 0);
		void SetGPUObjectConstantBuffer(const GPUBuffer& buffer, std::uint64_t offset = 0);
		void SetGPUMaterialConstantBuffer(const GPUBuffer& buffer, std::uint64_t offset = 0);
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
			const GPUDevice& device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList,
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator) noexcept;

		[[nodiscard]] std::expected<std::pair<const GPUBuffer*, std::uint64_t>, std::string> UploadConstants(
			std::span<const std::byte> bytes);

		const GPUDevice* device;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
		std::vector<GPUBuffer> constantBufferPages;
		std::size_t constantBufferPageIndex = 0;
		std::uint64_t constantBufferOffset = 0;
		DXGI_FORMAT activeTargetFormat = DXGI_FORMAT_UNKNOWN;
		bool cameraPrepared = false;
	};
}
