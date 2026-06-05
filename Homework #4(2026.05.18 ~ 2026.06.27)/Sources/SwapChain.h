#pragma once

#include <vector>

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

class DescriptorAllocator;

class SwapChain
{
public:
	SwapChain() noexcept = default;
	~SwapChain() noexcept = default;

	bool Initialize(IDXGIFactory4* factory_, ID3D12CommandQueue* commandQueue_, HWND window_);
	void Release();

private:
	UINT clientWidth{ 0 };
	UINT clientHeight{ 0 };

	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};
};
