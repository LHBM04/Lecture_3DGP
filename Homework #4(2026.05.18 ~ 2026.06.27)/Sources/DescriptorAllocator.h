#pragma once

#include <wrl.h>

#include <d3d12.h>

class DescriptorAllocatorOptions final
{
public:
	ID3D12Device* device;
	D3D12_DESCRIPTOR_HEAP_TYPE type;
	D3D12_DESCRIPTOR_HEAP_FLAGS flags;
	UINT numDescriptors;
};

class DescriptorAllocator
{
public:
	DescriptorAllocator() noexcept = default;
	~DescriptorAllocator() noexcept = default;

	bool Initialize(const DescriptorAllocatorOptions& options_);
	void Release();

	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index_) const noexcept;
	[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index_) const noexcept;
	[[nodiscard]] ID3D12DescriptorHeap& GetHeap() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
	UINT descriptorSize{ 0 };
};
