#pragma once

#include <expected>
#include <string>

#include <cstddef>
#include <d3d12.h>
#include <span>
#include <string>
#include <wrl.h>

namespace TUK::Framework
{
	class GraphicsDevice;
	class RenderContext;

	class Buffer
	{
		friend class GraphicsDevice;
		friend class RenderContext;

	public:
		~Buffer() noexcept = default;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;
		Buffer(Buffer&&) noexcept = default;
		Buffer& operator=(Buffer&&) noexcept = default;

		[[nodiscard]] UINT64 GetSize() const noexcept;
		[[nodiscard]] D3D12_HEAP_TYPE GetHeapType() const noexcept;
		[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept;
		[[nodiscard]] std::expected<D3D12_VERTEX_BUFFER_VIEW, std::string> GetVertexBufferView(UINT stride) const;
		[[nodiscard]] std::expected<D3D12_INDEX_BUFFER_VIEW, std::string> GetIndexBufferView(DXGI_FORMAT format) const;

		/** UPLOAD 버퍼에 기록한다. GPU가 같은 영역을 사용하는 동안 호출하지 않는다. */
		[[nodiscard]] std::expected<void, std::string> Write(std::span<const std::byte> data, std::size_t offset = 0);
		/** READBACK 버퍼에서 읽는다. 호출 전에 Fence로 GPU 복사 완료를 확인한다. */
		[[nodiscard]] std::expected<void, std::string> Read(std::span<std::byte> destination, std::size_t offset = 0) const;
		/** GPU가 버퍼 사용을 마친 뒤 호출한다. */
		void Release() noexcept;

	private:
		Buffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource, UINT64 size, D3D12_HEAP_TYPE heapType) noexcept;
		[[nodiscard]] ID3D12Resource& GetResource() const noexcept;
		[[nodiscard]] std::expected<void, std::string> CheckRange(std::size_t offset, std::size_t length) const;

		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		UINT64 size;
		D3D12_HEAP_TYPE heapType;
	};
}
