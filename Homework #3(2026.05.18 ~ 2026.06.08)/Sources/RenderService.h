#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Service.h"

struct RendererOptions final
{

};

class RenderService : public Service
{
public:
	~RenderService() override = default;

    bool AddTarget(HWND window_);
    void RemoveTarget(HWND window_);

    void Render();

protected:
	void OnAdd() override;
	void OnRemove() override;

private:
	struct RenderTarget final
	{
        int width{ 0 };
        int height{ 0 };

        Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
        Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers[2];
        Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        UINT64 fenceValues[2]{ 0 };
        HANDLE fenceEvent{ nullptr };

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};

        UINT frameIndex{ 0 };
	};

    bool CreateDevice();
    bool CreateCommandObjects();
    bool CreateDescriptorHeaps();

    void WaitForGpu(RenderTarget& context_);

    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[2];
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
    UINT rtvDescriptorSize{ 0 };
    UINT rtvHeapOffset{ 0 };

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
    UINT dsvDescriptorSize{ 0 };
    UINT dsvHeapOffset{ 0 };

    UINT globalFrameIndex{ 0 };

    std::unordered_map<HWND, std::unique_ptr<RenderTarget>> renderTargets;
};