#pragma once

#include <expected>
#include <span>
#include <string>

#include <directx/d3d12.h>

#include <wrl/client.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	struct GPUBufferProperties final
	{
		ComPtr<ID3D12Resource> d3dResource;
		D3D12_HEAP_TYPE heapType;
	};

	class GPUBuffer
	{
	public:
		explicit GPUBuffer(const GPUBufferProperties& properties) noexcept;
		~GPUBuffer() noexcept = default;

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

	private:
		ComPtr<ID3D12Resource> d3dResource;
		D3D12_HEAP_TYPE heapType;
	};

	inline GPUBuffer::GPUBuffer(const GPUBufferProperties& properties) noexcept
		: d3dResource(properties.d3dResource)
		, heapType(properties.heapType)
	{
	}

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
}
