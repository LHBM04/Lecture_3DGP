#pragma once

#include <cstddef>

#include <wrl.h>

#include <d3d12.h>

class UploadBuffer
{
public:
	UploadBuffer() noexcept = default;
	~UploadBuffer() noexcept = default;

	bool Initialize(ID3D12Device* device_, UINT bufferSize_);
	void Release();

	void Reset();

	template <class TData>
	[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS Allocate(const TData& data_, UINT sizeInBytes_);

private:
	UINT GetConstantBufferSize(UINT sizeInBytes_) const noexcept;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	std::byte* mappedData{ nullptr };
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ 0 };
	UINT bufferSize{ 0 };
	UINT currentOffset{ 0 };
};

template <class TData>
inline D3D12_GPU_VIRTUAL_ADDRESS UploadBuffer::Allocate(const TData& data_, UINT sizeInBytes_)
{
	const UINT alignedSize = GetConstantBufferSize(sizeInBytes_);
	if (currentOffset + alignedSize > bufferSize)
	{
		return 0;
	}
	const D3D12_GPU_VIRTUAL_ADDRESS address = gpuAddress + currentOffset;
	std::memcpy(mappedData + currentOffset, &data_, sizeInBytes_);
	currentOffset += alignedSize;
	return address;
}
