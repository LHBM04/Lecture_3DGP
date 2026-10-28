#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	struct GPUBufferDescription final
	{
		std::uint64_t sizeInBytes = 0;
		D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;

		// DEFAULT에 적용합니다. UPLOAD는 GENERIC_READ, READBACK은 COPY_DEST로 고정합니다.
		D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
	};

	class GPUBuffer
	{
		friend class GPUDevice;

	public:
		~GPUBuffer() noexcept = default;

		GPUBuffer(const GPUBuffer&) = delete;
		GPUBuffer& operator=(const GPUBuffer&) = delete;

		GPUBuffer(GPUBuffer&&) noexcept = default;
		GPUBuffer& operator=(GPUBuffer&&) noexcept = default;

		[[nodiscard]] std::expected<void, std::string> Write(std::span<const std::byte> data, std::uint64_t offset = 0);
		[[nodiscard]] std::expected<void, std::string> Read(std::span<std::byte> data, std::uint64_t offset = 0) const;

		[[nodiscard]] std::uint64_t GetSizeInBytes() const noexcept;
		[[nodiscard]] D3D12_HEAP_TYPE GetD3DHeapType() const noexcept;
		[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept;
		[[nodiscard]] ID3D12Resource& GetD3DResource() const noexcept;

	private:
		explicit GPUBuffer(
			Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource,
			D3D12_HEAP_TYPE heapType) noexcept;

		Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource;
		D3D12_HEAP_TYPE heapType;
	};
}
