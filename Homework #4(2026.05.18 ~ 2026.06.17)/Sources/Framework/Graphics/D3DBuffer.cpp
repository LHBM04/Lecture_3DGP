#include "Precompiled.hpp"
#include "D3DBuffer.hpp"

namespace TUK::Framework
{
	std::expected<void, std::string> D3DBuffer::Write(std::span<const std::byte> data, std::uint64_t offset)
	{
		if (!d3dResource || heapType != D3D12_HEAP_TYPE_UPLOAD)
		{
			return std::unexpected("Write는 유효한 UPLOAD 버퍼에서만 사용할 수 있습니다.");
		}

		const std::uint64_t size = GetSizeInBytes();
		if (offset > size || data.size() > size - offset)
		{
			return std::unexpected("D3DBuffer Write 범위를 초과했습니다.");
		}

		if (data.empty())
		{
			return {};
		}

		void* mappedData = nullptr;
		const D3D12_RANGE readRange{ 0, 0 };
		if (FAILED(d3dResource->Map(0, &readRange, &mappedData)))
		{
			return std::unexpected("D3DBuffer Write Map에 실패했습니다.");
		}

		const auto begin = static_cast<std::size_t>(offset);
		std::memcpy(static_cast<std::byte*>(mappedData) + begin, data.data(), data.size());

		const D3D12_RANGE writtenRange{ begin, begin + data.size() };
		d3dResource->Unmap(0, &writtenRange);
		return {};
	}

	std::expected<void, std::string> D3DBuffer::Read(std::span<std::byte> data, std::uint64_t offset) const
	{
		if (!d3dResource || heapType != D3D12_HEAP_TYPE_READBACK)
		{
			return std::unexpected("Read는 유효한 READBACK 버퍼에서만 사용할 수 있습니다.");
		}

		const std::uint64_t size = GetSizeInBytes();
		if (offset > size || data.size() > size - offset)
		{
			return std::unexpected("D3DBuffer Read 범위를 초과했습니다.");
		}

		if (data.empty())
		{
			return {};
		}

		const auto begin = static_cast<std::size_t>(offset);
		const D3D12_RANGE readRange{ begin, begin + data.size() };
		void* mappedData = nullptr;
		if (FAILED(d3dResource->Map(0, &readRange, &mappedData)))
		{
			return std::unexpected("D3DBuffer Read Map에 실패했습니다.");
		}

		std::memcpy(data.data(), static_cast<const std::byte*>(mappedData) + begin, data.size());

		const D3D12_RANGE writtenRange{ 0, 0 };
		d3dResource->Unmap(0, &writtenRange);
		return {};
	}

	std::uint64_t D3DBuffer::GetSizeInBytes() const noexcept
	{
		return d3dResource->GetDesc().Width;
	}

	D3D12_HEAP_TYPE D3DBuffer::GetD3DHeapType() const noexcept
	{
		return heapType;
	}

	D3D12_GPU_VIRTUAL_ADDRESS D3DBuffer::GetGPUVirtualAddress() const noexcept
	{
		return d3dResource->GetGPUVirtualAddress();
	}

	ID3D12Resource& D3DBuffer::GetD3DResource() const noexcept
	{
		return *d3dResource.Get();
	}

	D3DBuffer::D3DBuffer(
		Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource,
		D3D12_HEAP_TYPE heapType) noexcept
		: d3dResource(std::move(d3dResource))
		, heapType(heapType)
	{
	}
}
