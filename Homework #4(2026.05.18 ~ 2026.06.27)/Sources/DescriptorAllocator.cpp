#include "Precompiled.h"
#include "DescriptorAllocator.h"

bool DescriptorAllocator::Initialize(const DescriptorAllocatorOptions& options_)
{
	if (options_.device == nullptr || options_.numDescriptors == 0)
	{
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = options_.type;
	descriptorHeapDesc.NumDescriptors = options_.numDescriptors;
	descriptorHeapDesc.Flags = options_.flags;
	descriptorHeapDesc.NodeMask = 0;

	if (FAILED(options_.device->CreateDescriptorHeap(
		&descriptorHeapDesc,
		IID_PPV_ARGS(heap.GetAddressOf()))))
	{
		return false;
	}

	descriptorSize = options_.device->GetDescriptorHandleIncrementSize(options_.type);
	return true;
}

void DescriptorAllocator::Release()
{
	heap.Reset();
	descriptorSize = 0;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::GetCpuHandle(UINT index_) const noexcept
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle{ heap->GetCPUDescriptorHandleForHeapStart() };
	handle.ptr += static_cast<SIZE_T>(index_) * descriptorSize;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorAllocator::GetGpuHandle(UINT index_) const noexcept
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle{ heap->GetGPUDescriptorHandleForHeapStart() };
	handle.ptr += static_cast<UINT64>(index_) * descriptorSize;
	return handle;
}

ID3D12DescriptorHeap& DescriptorAllocator::GetHeap() const noexcept
{
	assert(heap != nullptr);
	return *heap.Get();
}
