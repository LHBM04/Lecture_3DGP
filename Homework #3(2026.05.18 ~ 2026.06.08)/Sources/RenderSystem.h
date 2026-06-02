#pragma once

#include <cstddef>

#include "Singleton.h"

class RenderSystem : public Singleton<RenderSystem>
{
public:
	static constexpr std::size_t BufferOffset{ 256 };
	
	struct alignas(BufferOffset) CameraConstants final
	{

	};

	struct alignas(BufferOffset) LightConstants final
	{

	};

	struct alignas(BufferOffset) ObjectConstants final
	{

	};

	struct alignas(BufferOffset) MaterialConstants final
	{

	};

	virtual ~RenderSystem() = default;

	bool Initialize(HWND window_);
	void Release();

private:
	static constexpr uint32_t BackBufferCount{ 2 };

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
#endif

	Microsoft::WRL::ComPtr<ID3D12Device> device;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[BackBufferCount];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	uint32_t frameIndex{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	uint32_t rtvDescriptorSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	uint32_t dsvDescriptorSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	uint32_t srvDescriptorSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[BackBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> gameObjectPipelineState;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> uiObjectPipelineState;

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	uint64_t fenceValues[BackBufferCount]{ 0 };
	HANDLE fenceEvent{ nullptr };
};
