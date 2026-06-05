#pragma once

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

class DescriptorAllocator
{
public:
	struct Options final
	{
		ID3D12Device* device;
		D3D12_DESCRIPTOR_HEAP_TYPE type;
		UINT numDescriptors;
	};

	DescriptorAllocator() noexcept = default;
	~DescriptorAllocator() noexcept = default;

	bool Initialize(const Options& options_);
	void Release();

	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index_) const noexcept;
	[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index_) const noexcept;
	[[nodiscard]] ID3D12DescriptorHeap* GetHeap() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
	UINT descriptorSize{ 0 };
};
