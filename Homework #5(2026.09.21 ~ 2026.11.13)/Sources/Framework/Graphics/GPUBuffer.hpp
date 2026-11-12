#pragma once

#include <wrl/client.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	struct GPUBufferCreateInfo final
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource;
		D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;
	};

	class GPUBuffer
	{
	public:
		explicit GPUBuffer(GPUBufferCreateInfo&& createInfo) noexcept;

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
		Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource;
		D3D12_HEAP_TYPE heapType;
	};

	inline GPUBuffer::GPUBuffer(GPUBufferCreateInfo&& createInfo) noexcept
		: d3dResource(std::move(createInfo.d3dResource))
		, heapType(createInfo.heapType)
	{
		ASSERT_FMT(this->d3dResource.Get(), "d3dResource가 nullptr입니다.");
	}

	inline u64 GPUBuffer::GetSizeInBytes() const noexcept
	{
		ASSERT_FMT(d3dResource.Get(), "d3dResource가 nullptr입니다.");

		return d3dResource->GetDesc().Width;
	}

	inline D3D12_HEAP_TYPE GPUBuffer::GetD3DHeapType() const noexcept
	{
		return heapType;
	}

	inline D3D12_GPU_VIRTUAL_ADDRESS GPUBuffer::GetGPUVirtualAddress() const noexcept
	{
		ASSERT_FMT(d3dResource.Get(), "d3dResource가 nullptr입니다.");

		return d3dResource->GetGPUVirtualAddress();
	}

	inline ID3D12Resource& GPUBuffer::GetD3DResource() const noexcept
	{
		ASSERT_FMT(d3dResource.Get(), "d3dResource가 nullptr입니다.");

		return *d3dResource.Get();
	}
}
