#pragma once

#include <directx/d3d12.h>

#include <wrl.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class Camera;
	class MaterialAsset;
	class Entity;
	class GPUDevice;
	class GPUTexture;
	class GPUBuffer;

	struct GPUCommandBufferProperties final
	{
		const ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		const ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
	};

	class GPUCommandBuffer
	{
	public:
		explicit GPUCommandBuffer(const GPUDevice& device, const GPUCommandBufferProperties& properties);
		~GPUCommandBuffer() noexcept = default;

		GPUCommandBuffer(const GPUCommandBuffer&) = delete;
		GPUCommandBuffer& operator=(const GPUCommandBuffer&) = delete;

		GPUCommandBuffer(GPUCommandBuffer&&) noexcept = default;
		GPUCommandBuffer& operator=(GPUCommandBuffer&&) noexcept = default;

		void Reset();
		void Submit();

		void TransitionTexture(const GPUTexture& texture, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
		void TransitionBuffer(const GPUBuffer& buffer, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

		void SetCamera(const Camera& camera) noexcept;
		void SetMaterial(const MaterialAsset& material) noexcept;
		void SetEntity(const Entity& entity) noexcept;

		void Draw(u32 vertexCount, u32 firstVertex = 0);
		void DrawIndexed(u32 indexCount, u32 firstIndex = 0, u32 baseVertex = 0);

		[[nodiscard]] ID3D12GraphicsCommandList10& GetD3DCommandList() const noexcept;
		[[nodiscard]] ID3D12CommandAllocator& GetD3DCommandAllocator() const noexcept;

	private:
		const GPUDevice* device;

		ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
	};

	inline GPUCommandBuffer::GPUCommandBuffer(const GPUDevice& device, const GPUCommandBufferProperties& properties)
		: device(&device)
		, d3dCommandList(properties.d3dCommandList)
		, d3dCommandAllocator(properties.d3dCommandAllocator)
	{
	}

	inline ID3D12GraphicsCommandList10& GPUCommandBuffer::GetD3DCommandList() const noexcept
	{
		return *d3dCommandList.Get();
	}

	inline ID3D12CommandAllocator& GPUCommandBuffer::GetD3DCommandAllocator() const noexcept
	{
		return *d3dCommandAllocator.Get();
	}
}
