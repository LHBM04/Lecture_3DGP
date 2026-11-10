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

#include "GPUBuffer.hpp"
#include "GPUGraphicsPipelineState.hpp"

namespace TUK::Framework
{
	class GPUDevice;
	class GPUTarget;
	class GPUTexture2D;

	struct GPUCommandBufferCreateInfo final
	{
		const GPUDevice* device = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
	};

	class GPUCommandBuffer
	{
	public:
		static constexpr u32 TEXTURE_SLOT_COUNT = 8;

		explicit GPUCommandBuffer(GPUCommandBufferCreateInfo&& createInfo) noexcept;

		GPUCommandBuffer(const GPUCommandBuffer&) = delete;
		GPUCommandBuffer& operator=(const GPUCommandBuffer&) = delete;

		GPUCommandBuffer(GPUCommandBuffer&&) noexcept = default;
		GPUCommandBuffer& operator=(GPUCommandBuffer&&) noexcept = default;

		[[nodiscard]] std::expected<void, std::string> Reset();
		[[nodiscard]] std::expected<void, std::string> Submit();

		[[nodiscard]] std::expected<void, std::string> SetGraphicsConstants(
			u32 rootSlot, std::span<const std::byte> bytes);
		[[nodiscard]] std::expected<void, std::string> BindTextures(
			std::span<const GPUTexture2D* const, TEXTURE_SLOT_COUNT> textures,
			const D3D12_SHADER_RESOURCE_VIEW_DESC* firstView = nullptr);

		void TransitionTexture(
			const GPUTexture2D& texture,
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

		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology) noexcept;
		void SetViewport(const D3D12_VIEWPORT& viewport) noexcept;
		void SetScissorRect(const D3D12_RECT& scissorRect) noexcept;

		void SetVertexBuffer(u32 slot, const D3D12_VERTEX_BUFFER_VIEW& view) noexcept;
		void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& view) noexcept;

		void Draw(u32 vertexCount, u32 firstVertex = 0);
		void DrawIndexed(u32 indexCount, u32 firstIndex = 0, i32 baseVertex = 0);

		void ClearRenderTarget(const f32 (&color)[4], const D3D12_RECT& rect) noexcept;
		void ClearDepth(f32 depth, const D3D12_RECT& rect) noexcept;
		void SetGPUTarget(const GPUTarget& target) noexcept;

		[[nodiscard]] DXGI_FORMAT GetActiveTargetFormat() const noexcept;
		[[nodiscard]] const D3D12_RECT& GetActiveTargetBounds() const noexcept;
		[[nodiscard]] const D3D12_VIEWPORT& GetActiveViewport() const noexcept;
		[[nodiscard]] ID3D12GraphicsCommandList10& GetD3DCommandList() const noexcept;
		[[nodiscard]] ID3D12CommandAllocator& GetD3DCommandAllocator() const noexcept;

	private:
		[[nodiscard]] std::expected<std::pair<const GPUBuffer*, u64>, std::string> UploadConstants(
			std::span<const std::byte> bytes);

		const GPUDevice* device;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
		
		std::vector<GPUBuffer> constantBufferPages;
		usize constantBufferPageIndex = 0;
		u64 constantBufferOffset = 0;

		std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> textureDescriptorPages;
		usize textureDescriptorCount = 0;
		
		DXGI_FORMAT activeTargetFormat = DXGI_FORMAT_UNKNOWN;
		D3D12_CPU_DESCRIPTOR_HANDLE activeRTV{};
		D3D12_CPU_DESCRIPTOR_HANDLE activeDSV{};
		D3D12_RECT activeTargetBounds{};
		D3D12_VIEWPORT activeViewport{};
	};

	inline GPUCommandBuffer::GPUCommandBuffer(GPUCommandBufferCreateInfo&& createInfo) noexcept
		: device(createInfo.device)
		, d3dCommandList(std::move(createInfo.d3dCommandList))
		, d3dCommandAllocator(std::move(createInfo.d3dCommandAllocator))
	{
		ASSERT_FMT(this->device, "device가 nullptr입니다.");
		ASSERT_FMT(this->d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		ASSERT_FMT(this->d3dCommandAllocator.Get(), "d3dCommandAllocator가 nullptr입니다.");
	}

	inline DXGI_FORMAT GPUCommandBuffer::GetActiveTargetFormat() const noexcept
	{
		return activeTargetFormat;
	}

	inline const D3D12_RECT& GPUCommandBuffer::GetActiveTargetBounds() const noexcept
	{
		return activeTargetBounds;
	}

	inline const D3D12_VIEWPORT& GPUCommandBuffer::GetActiveViewport() const noexcept
	{
		return activeViewport;
	}

	inline ID3D12GraphicsCommandList10& GPUCommandBuffer::GetD3DCommandList() const noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		return *d3dCommandList.Get();
	}

	inline ID3D12CommandAllocator& GPUCommandBuffer::GetD3DCommandAllocator() const noexcept
	{
		ASSERT_FMT(d3dCommandAllocator.Get(), "d3dCommandAllocator가 nullptr입니다.");
		return *d3dCommandAllocator.Get();
	}
}
