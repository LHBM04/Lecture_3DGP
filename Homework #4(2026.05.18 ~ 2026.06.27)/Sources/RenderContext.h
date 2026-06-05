#pragma once

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

class RenderContext
{
public:
	RenderContext() noexcept = default;
	~RenderContext() noexcept = default;

	bool Initialize(ID3D12Device* device_, UINT backBufferCount_);
	void Release();

	void Reset(UINT frameIndex_);
	void Execute();

	void MoveToNextFrame(UINT& frameIndex_, IDXGISwapChain3* swapChain_);
	void WaitForGpu(UINT frameIndex_);

	[[nodiscard]] ID3D12CommandQueue* GetCommandQueue() const noexcept;
	[[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> commandAllocators;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	std::vector<UINT64> fenceValues;
	HANDLE fenceEvent{ nullptr };
};
