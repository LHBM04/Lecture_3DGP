#include "Precompiled.h"
#include "Buffer.h"

#include "GraphicsError.h"

namespace TUK::Framework
{
	Buffer::Buffer(Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource, UINT64 bufferSize,
		D3D12_HEAP_TYPE bufferHeapType) noexcept
		: resource(std::move(bufferResource))
		, size(bufferSize)
		, heapType(bufferHeapType)
	{
		assert(resource);
	}

	Buffer::Buffer(Buffer&& other) noexcept
		: resource(std::move(other.resource))
		, size(other.size)
		, heapType(other.heapType)
	{
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		if (this != &other)
		{
			resource = std::move(other.resource);
			size = other.size;
			heapType = other.heapType;
		}
		return *this;
	}

	ID3D12Resource& Buffer::GetResource() const noexcept
	{
		assert(resource);
		return *resource.Get();
	}

	UINT64 Buffer::GetSize() const noexcept
	{
		assert(resource);
		return size;
	}

	D3D12_HEAP_TYPE Buffer::GetHeapType() const noexcept
	{
		assert(resource);
		return heapType;
	}

	D3D12_GPU_VIRTUAL_ADDRESS Buffer::GetGPUVirtualAddress() const noexcept
	{
		return GetResource().GetGPUVirtualAddress();
	}

	std::expected<D3D12_VERTEX_BUFFER_VIEW, std::string> Buffer::GetVertexBufferView(UINT stride) const
	{
		assert(resource);
		if (stride == 0 || stride > D3D12_REQ_MULTI_ELEMENT_STRUCTURE_SIZE_IN_BYTES
			|| stride > size || size > (std::numeric_limits<UINT>::max)())
		{
			return std::unexpected(std::string("정점 버퍼 뷰의 크기 또는 stride가 유효하지 않습니다."));
		}
		return D3D12_VERTEX_BUFFER_VIEW{ GetGPUVirtualAddress(), static_cast<UINT>(size), stride };
	}

	std::expected<D3D12_INDEX_BUFFER_VIEW, std::string> Buffer::GetIndexBufferView(DXGI_FORMAT format) const
	{
		assert(resource);
		if (format != DXGI_FORMAT_R16_UINT && format != DXGI_FORMAT_R32_UINT)
		{
			return std::unexpected(std::string("인덱스 포맷은 R16_UINT 또는 R32_UINT여야 합니다."));
		}
		const UINT indexSize = format == DXGI_FORMAT_R16_UINT ? 2 : 4;
		if (size > (std::numeric_limits<UINT>::max)() || size % indexSize != 0)
		{
			return std::unexpected(std::string("인덱스 버퍼 뷰의 크기가 유효하지 않습니다."));
		}
		return D3D12_INDEX_BUFFER_VIEW{ GetGPUVirtualAddress(), static_cast<UINT>(size), format };
	}

	std::expected<void, std::string> Buffer::CheckRange(std::size_t offset, std::size_t length) const
	{
		assert(resource);
		if (offset > size || length > size - offset
			|| length > (std::numeric_limits<std::size_t>::max)() - offset)
		{
			return std::unexpected(std::string("버퍼 접근 범위를 벗어났습니다."));
		}
		return {};
	}

	std::expected<void, std::string> Buffer::Write(std::span<const std::byte> data, std::size_t offset)
	{
		assert(resource);
		if (heapType != D3D12_HEAP_TYPE_UPLOAD)
		{
			return std::unexpected(std::string("Write는 UPLOAD 버퍼에서만 사용할 수 있습니다."));
		}
		const auto range = CheckRange(offset, data.size());
		if (!range)
		{
			return range;
		}
		if (data.empty())
		{
			return {};
		}
		void* mapped = nullptr;
		const D3D12_RANGE readRange{ 0, 0 };
		const HRESULT result = resource->Map(0, &readRange, &mapped);
		if (FAILED(result))
		{
			return CheckHResult(result, "버퍼 쓰기 매핑");
		}
		std::memcpy(static_cast<std::byte*>(mapped) + offset, data.data(), data.size());
		const D3D12_RANGE writtenRange{ offset, offset + data.size() };
		resource->Unmap(0, &writtenRange);
		return {};
	}

	std::expected<void, std::string> Buffer::Read(std::span<std::byte> destination, std::size_t offset) const
	{
		assert(resource);
		if (heapType != D3D12_HEAP_TYPE_READBACK)
		{
			return std::unexpected(std::string("Read는 READBACK 버퍼에서만 사용할 수 있습니다."));
		}
		const auto range = CheckRange(offset, destination.size());
		if (!range)
		{
			return range;
		}
		if (destination.empty())
		{
			return {};
		}
		void* mapped = nullptr;
		const D3D12_RANGE readRange{ offset, offset + destination.size() };
		const HRESULT result = resource->Map(0, &readRange, &mapped);
		if (FAILED(result))
		{
			return CheckHResult(result, "버퍼 읽기 매핑");
		}
		std::memcpy(destination.data(), static_cast<const std::byte*>(mapped) + offset, destination.size());
		const D3D12_RANGE writtenRange{ 0, 0 };
		resource->Unmap(0, &writtenRange);
		return {};
	}

	void Buffer::Release() noexcept
	{
		resource.Reset();
		size = 0;
		heapType = D3D12_HEAP_TYPE_DEFAULT;
	}
}
