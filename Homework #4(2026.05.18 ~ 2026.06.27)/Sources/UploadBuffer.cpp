#include "Precompiled.h"
#include "UploadBuffer.h"

bool UploadBuffer::Initialize(const UploadBufferOptions& options_)
{
	if (options_.device == nullptr || options_.bufferSize == 0)
	{
		return false;
	}

	bufferSize = options_.bufferSize;

	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProperties.CreationNodeMask = 1;
	uploadHeapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC bufferDesc{};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Alignment = 0;
	bufferDesc.Width = bufferSize;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	if (FAILED(options_.device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(resource.GetAddressOf()))))
	{
		return false;
	}

	if (FAILED(resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData))))
	{
		return false;
	}

	gpuAddress = resource->GetGPUVirtualAddress();
	currentOffset = 0;
	return true;
}

void UploadBuffer::Release()
{
	if (resource != nullptr)
	{
		resource->Unmap(0, nullptr);
	}

	resource.Reset();
	mappedData = nullptr;
	gpuAddress = 0;
	bufferSize = 0;
	currentOffset = 0;
}

void UploadBuffer::Reset()
{
	currentOffset = 0;
}

UINT UploadBuffer::GetConstantBufferSize(UINT sizeInBytes_) const noexcept
{
	return (sizeInBytes_ + 255U) & ~255U;
}
