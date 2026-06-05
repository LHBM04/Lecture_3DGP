#pragma once

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

class UploadBuffer;

class RenderContextOptions final
{
public:
	ID3D12Device* device;
	UINT backBufferCount;
};

class RenderContext
{
public:
	RenderContext() noexcept = default;
	~RenderContext() noexcept = default;

	bool Initialize(const RenderContextOptions& options_);
	void Release();

	void Reset(UINT frameIndex_);
	void Execute();

	void SetUploadBuffer(UploadBuffer& uploadBuffer_) noexcept;
	void SetRenderTargetHandles(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_) noexcept;

	void MoveToNextFrame(UINT& frameIndex_, IDXGISwapChain3& swapChain_);
	void WaitForGpu(UINT frameIndex_);

	[[nodiscard]] ID3D12CommandQueue& GetCommandQueue() const noexcept;
	[[nodiscard]] ID3D12GraphicsCommandList& GetCommandList() const noexcept;
	[[nodiscard]] UploadBuffer& GetUploadBuffer() const noexcept;
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle() const noexcept;
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> commandAllocators;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	std::vector<UINT64> fenceValues;
	HANDLE fenceEvent{ nullptr };

	UploadBuffer* uploadBuffer{ nullptr };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
};
