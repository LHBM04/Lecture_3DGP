#pragma once

#include <vector>

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

class DescriptorAllocator;

class SwapChainOptions final
{
public:
	IDXGIFactory4* factory;
	ID3D12Device* device;
	ID3D12CommandQueue* commandQueue;
	HWND window;
	UINT backBufferCount;
	DescriptorAllocator* rtvAllocator;
	DescriptorAllocator* dsvAllocator;
};

class SwapChain
{
public:
	SwapChain() noexcept = default;
	~SwapChain() noexcept = default;

	bool Initialize(const SwapChainOptions& options_);
	void Release();

	[[nodiscard]] IDXGISwapChain3& GetDXGISwapChain() const noexcept;
	[[nodiscard]] ID3D12Resource& GetCurrentBackBuffer(UINT frameIndex_) const noexcept;
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle(UINT frameIndex_) const noexcept;
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() const noexcept;
	[[nodiscard]] const D3D12_VIEWPORT& GetViewport() const noexcept;
	[[nodiscard]] const D3D12_RECT& GetScissorRect() const noexcept;

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
