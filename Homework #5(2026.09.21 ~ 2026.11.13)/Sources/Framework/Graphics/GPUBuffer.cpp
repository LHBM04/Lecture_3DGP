#include "Precompiled.hpp"
#include "GPUBuffer.hpp"

#include <cstring>
#include <limits>

#include "GPUDevice.hpp"

namespace TUK::Framework
{
	std::expected<GPUBuffer, std::string> GPUBuffer::Create(
		const GPUDevice& device,
		D3D12_HEAP_TYPE heapType,
		u64 sizeInBytes,
		D3D12_RESOURCE_STATES initialState,
		D3D12_RESOURCE_FLAGS flags)
	{
		if (sizeInBytes == 0 || sizeInBytes > std::numeric_limits<usize>::max())
		{
			return std::unexpected("GPU 버퍼 크기가 유효하지 않습니다.");
		}

		const CD3DX12_HEAP_PROPERTIES heapProperties(heapType);
		const auto description = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes, flags);

		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		if (FAILED(device.GetD3DDevice().CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&description,
			initialState,
			nullptr,
			IID_PPV_ARGS(resource.GetAddressOf()))))
		{
			return std::unexpected("GPU 버퍼 리소스 생성에 실패했습니다.");
		}

		return GPUBuffer(std::move(resource), heapType);
	}

	std::expected<GPUBuffer, std::string> GPUBuffer::CreateUpload(
		const GPUDevice& device,
		std::span<const std::byte> data)
	{
		auto buffer = Create(device, D3D12_HEAP_TYPE_UPLOAD, data.size(), D3D12_RESOURCE_STATE_GENERIC_READ);
		if (!buffer)
		{
			return std::unexpected(buffer.error());
		}

		if (auto result = buffer->Write(std::span<const u8>(reinterpret_cast<const u8*>(data.data()), data.size())); !result)
		{
			return std::unexpected(result.error());
		}

		return buffer;
	}

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
