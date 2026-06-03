#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "RenderContext.h"
#include "RendererOptions.h"
#include "RenderTarget.h"
#include "Service.h"

class RenderService : public Service
{
public:
	~RenderService() override = default;

    bool AddTarget(HWND window_);
    void RemoveTarget(HWND window_);

    void BeginFrame();
    void Render(HWND window_, const RenderContext& renderContext_);
    void EndFrame();

protected:
	void OnAdd() override;
	void OnRemove() override;

private:
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
