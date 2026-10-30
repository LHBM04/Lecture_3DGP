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
	class D3DBuffer
	{
		friend class D3DDevice;

	public:
		~D3DBuffer() noexcept = default;

		D3DBuffer(const D3DBuffer&) = delete;
		D3DBuffer& operator=(const D3DBuffer&) = delete;

		D3DBuffer(D3DBuffer&&) noexcept = default;
		D3DBuffer& operator=(D3DBuffer&&) noexcept = default;

		[[nodiscard]] std::expected<void, std::string> Write(std::span<const std::byte> data, std::uint64_t offset = 0);
		[[nodiscard]] std::expected<void, std::string> Read(std::span<std::byte> data, std::uint64_t offset = 0) const;

		[[nodiscard]] std::uint64_t GetSizeInBytes() const noexcept;
		[[nodiscard]] D3D12_HEAP_TYPE GetD3DHeapType() const noexcept;
		[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept;
		[[nodiscard]] ID3D12Resource& GetD3DResource() const noexcept;

	private:
		explicit D3DBuffer(
			Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource,
			D3D12_HEAP_TYPE heapType) noexcept;

		Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource;
		D3D12_HEAP_TYPE heapType;
	};
}
