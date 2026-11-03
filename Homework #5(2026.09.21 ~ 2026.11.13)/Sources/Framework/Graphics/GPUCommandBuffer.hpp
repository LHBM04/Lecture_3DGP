#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <wrl/client.h>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

#include "../Math/ColorRGBA.hpp"
#include "../Math/Matrix4x4.hpp"

#include "GPUBuffer.hpp"
#include "GPUGraphicsPipelineState.hpp"

namespace TUK::Framework
{
	class Camera;
	class Entity;
	class GPUDevice;
	class GPUMesh;
	class GPUTarget;
	class GPUTexture;
	class MaterialAsset;

	class GPUCommandBuffer
	{
	public:
		~GPUCommandBuffer() noexcept = default;

		GPUCommandBuffer(const GPUCommandBuffer&) = delete;
		GPUCommandBuffer& operator=(const GPUCommandBuffer&) = delete;

		GPUCommandBuffer(GPUCommandBuffer&&) noexcept = default;
		GPUCommandBuffer& operator=(GPUCommandBuffer&&) noexcept = default;

		[[nodiscard]] std::expected<void, std::string> Reset();
		[[nodiscard]] std::expected<void, std::string> Submit();

		[[nodiscard]] std::expected<void, std::string> SetEntity(const Entity& entity) noexcept;
		[[nodiscard]] std::expected<void, std::string> SetCamera(const Camera& camera) noexcept;
		[[nodiscard]] std::expected<void, std::string> SetMaterial(const MaterialAsset& material) noexcept;
		[[nodiscard]] std::expected<void, std::string> SetObjectTransform(const Matrix4x4<f32>& worldMatrix) noexcept;

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
			u64 destinationOffset,
			const GPUBuffer& source,
			u64 sourceOffset,
			u64 sizeInBytes);

		void SetGPUGraphicsPipelineState(const GPUGraphicsPipelineState& pipelineState) noexcept;

		void SetGPUCameraConstantBuffer(const GPUBuffer& buffer, u64 offset = 0) noexcept;
		void SetGPUObjectConstantBuffer(const GPUBuffer& buffer, u64 offset = 0) noexcept;
		void SetGPUMaterialConstantBuffer(const GPUBuffer& buffer, u64 offset = 0) noexcept;
		void SetViewport(const D3D12_VIEWPORT& viewport) noexcept;
		void SetScissorRect(const D3D12_RECT& scissorRect) noexcept;

		void SetGPUMesh(const GPUMesh& mesh) noexcept;

		void Draw(u32 vertexCount, u32 firstVertex = 0);
		void DrawIndexed(u32 indexCount, u32 firstIndex = 0, i32 baseVertex = 0);

		void ClearGPUTarget(const GPUTarget& target, const ColorRGBA<f32>& clearColor);
		void SetGPUTarget(const GPUTarget& target) noexcept;

		[[nodiscard]] ID3D12GraphicsCommandList10& GetD3DCommandList() const noexcept;
		[[nodiscard]] ID3D12CommandAllocator& GetD3DCommandAllocator() const noexcept;

		[[nodiscard]] static std::expected<GPUCommandBuffer, std::string> Create(const GPUDevice& device);

	private:
		explicit GPUCommandBuffer(
			const GPUDevice& device,
			ComPtr<ID3D12GraphicsCommandList10> d3dCommandList,
			ComPtr<ID3D12CommandAllocator> d3dCommandAllocator) noexcept;

		[[nodiscard]] std::expected<std::pair<const GPUBuffer*, u64>, std::string> UploadConstants(
			std::span<const std::byte> bytes);

		const GPUDevice* device;
		ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
		std::vector<GPUBuffer> constantBufferPages;
		usize constantBufferPageIndex = 0;
		u64 constantBufferOffset = 0;
		DXGI_FORMAT activeTargetFormat = DXGI_FORMAT_UNKNOWN;
		bool cameraPrepared = false;
	};

	inline ID3D12GraphicsCommandList10& GPUCommandBuffer::GetD3DCommandList() const noexcept
	{
		return *d3dCommandList.Get();
	}

	inline ID3D12CommandAllocator& GPUCommandBuffer::GetD3DCommandAllocator() const noexcept
	{
		return *d3dCommandAllocator.Get();
	}

	inline GPUCommandBuffer::GPUCommandBuffer(
		const GPUDevice& device,
		ComPtr<ID3D12GraphicsCommandList10> d3dCommandList,
		ComPtr<ID3D12CommandAllocator> d3dCommandAllocator) noexcept
		: device(&device)
		, d3dCommandList(std::move(d3dCommandList))
		, d3dCommandAllocator(std::move(d3dCommandAllocator))
	{
	}
}
