#include "Precompiled.hpp"
#include "GPUBuffer.hpp"

namespace TUK::Framework
{
	std::expected<void, std::string> GPUBuffer::Write(std::span<const u8> data, u64 offset)
	{
		if (!d3dResource || heapType != D3D12_HEAP_TYPE_UPLOAD)
		{
			return std::unexpected("쓰기에 유효한 UPLOAD 버퍼가 필요합니다.");
		}

		const u64 size = GetSizeInBytes();
		if (offset > size || data.size() > size - offset)
		{
			return std::unexpected("GPU 버퍼 쓰기 범위가 유효 범위를 벗어났습니다.");
		}

		if (data.empty())
		{
			return {};
		}

		const u64 begin = offset;
		void* mappedData = nullptr;

		const CD3DX12_RANGE readRange{ 0, 0 };
		if (FAILED(d3dResource->Map(0, &readRange, &mappedData)))
		{
			return std::unexpected("쓰기를 위한 GPU 버퍼 매핑에 실패했습니다.");
		}

		std::memcpy(static_cast<u8*>(mappedData) + begin, data.data(), data.size());

		const CD3DX12_RANGE writtenRange{ begin, begin + data.size() };
		d3dResource->Unmap(0, &writtenRange);

		return {};
	}

	std::expected<void, std::string> GPUBuffer::Read(std::span<u8> data, u64 offset) const
	{
		if (!d3dResource || heapType != D3D12_HEAP_TYPE_READBACK)
		{
			return std::unexpected("읽기에 유효한 READBACK 버퍼가 필요합니다.");
		}

		const u64 size = GetSizeInBytes();
		if (offset > size || data.size() > size - offset)
		{
			return std::unexpected("GPU 버퍼 읽기 범위가 유효 범위를 벗어났습니다.");
		}

		if (data.empty())
		{
			return {};
		}

		const u64 begin = offset;
		void* mappedData = nullptr;

		const CD3DX12_RANGE readRange{ begin, begin + data.size() };
		if (FAILED(d3dResource->Map(0, &readRange, &mappedData)))
		{
			return std::unexpected("읽기를 위한 GPU 버퍼 매핑에 실패했습니다.");
		}

		std::memcpy(data.data(), static_cast<const u8*>(mappedData) + begin, data.size());

		const CD3DX12_RANGE writtenRange{ 0, 0 };
		d3dResource->Unmap(0, &writtenRange);

		return {};
	}
}
