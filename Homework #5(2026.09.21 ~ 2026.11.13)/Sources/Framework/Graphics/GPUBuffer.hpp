#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <utility>

#include <wrl/client.h>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class GPUBuffer
	{
	public:

		GPUBuffer(const GPUBuffer&) = delete;
		GPUBuffer& operator=(const GPUBuffer&) = delete;

		GPUBuffer(GPUBuffer&&) noexcept = default;
		GPUBuffer& operator=(GPUBuffer&&) noexcept = default;

		[[nodiscard]] std::expected<void, std::string> Write(std::span<const u8> data, u64 offset = 0);
		[[nodiscard]] std::expected<void, std::string> Read(std::span<u8> data, u64 offset = 0) const;

		[[nodiscard]] u64 GetSizeInBytes() const noexcept;
		[[nodiscard]] D3D12_HEAP_TYPE GetD3DHeapType() const noexcept;
		[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept;
		[[nodiscard]] ID3D12Resource& GetD3DResource() const noexcept;

		[[nodiscard]] static std::expected<GPUBuffer, std::string> Create(
			const GPUDevice& device,
			D3D12_HEAP_TYPE heapType,
			u64 sizeInBytes,
			D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

		[[nodiscard]] static std::expected<GPUBuffer, std::string> CreateUpload(
			const GPUDevice& device,
			std::span<const std::byte> data);

	private:
		GPUBuffer(ComPtr<ID3D12Resource> d3dResource, D3D12_HEAP_TYPE heapType) noexcept;

		ComPtr<ID3D12Resource> d3dResource;
		D3D12_HEAP_TYPE heapType;
	};

	inline u64 GPUBuffer::GetSizeInBytes() const noexcept
	{
		return d3dResource->GetDesc().Width;
	}

	inline D3D12_HEAP_TYPE GPUBuffer::GetD3DHeapType() const noexcept
	{
		return heapType;
	}

	inline D3D12_GPU_VIRTUAL_ADDRESS GPUBuffer::GetGPUVirtualAddress() const noexcept
	{
		return d3dResource->GetGPUVirtualAddress();
	}

	inline ID3D12Resource& GPUBuffer::GetD3DResource() const noexcept
	{
		return *d3dResource.Get();
	}

	inline GPUBuffer::GPUBuffer(ComPtr<ID3D12Resource> d3dResource, D3D12_HEAP_TYPE heapType) noexcept
		: d3dResource(std::move(d3dResource))
		, heapType(heapType)
	{
		ASSERT_FMT(this->d3dResource.Get(), "검증 실패: {}", "this->d3dResource.Get()");
	}
}
