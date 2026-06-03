#include "Precompiled.h"
#include "RenderService.h"

#include "Engine.h"

bool RenderService::AddTarget(HWND window_)
{
    if (renderTargets.contains(window_))
    {
        return true;
    }

    RECT clientRect{};
    ::GetClientRect(window_, &clientRect);

    std::unique_ptr<RenderTarget> target{ std::make_unique<RenderTarget>() };
    target->width = static_cast<int>(clientRect.right);
    target->height = static_cast<int>(clientRect.bottom);

    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Width = target->width;
    swapChainDesc.Height = target->height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
    dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), window_, &swapChainDesc, nullptr, nullptr, &swapChain1);
    swapChain1.As(&target->swapChain);
    target->frameIndex = target->swapChain->GetCurrentBackBufferIndex();

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
    rtvHandle.ptr += static_cast<SIZE_T>(rtvHeapOffset) * rtvDescriptorSize;

    for (UINT i{ 0 }; i < 2; ++i)
    {
        target->swapChain->GetBuffer(i, IID_PPV_ARGS(&target->backBuffers[i]));
        device->CreateRenderTargetView(target->backBuffers[i].Get(), nullptr, rtvHandle);
        target->rtvHandles[i] = rtvHandle;
        rtvHandle.ptr += rtvDescriptorSize;
        rtvHeapOffset++;
    }

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC depthDesc{};
    depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthDesc.Alignment = 0;
    depthDesc.Width = target->width;
    depthDesc.Height = target->height;
    depthDesc.DepthOrArraySize = 1;
    depthDesc.MipLevels = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE depthOptimizedClearValue{};
    depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(&target->depthStencilBuffer)
    );

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };
    dsvHandle.ptr += static_cast<SIZE_T>(dsvHeapOffset) * dsvDescriptorSize;
    device->CreateDepthStencilView(target->depthStencilBuffer.Get(), nullptr, dsvHandle);
    target->dsvHandle = dsvHandle;
    dsvHeapOffset++;

    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&target->fence));
    target->fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    renderTargets[window_] = std::move(target);
    return true;
}

void RenderService::RemoveTarget(HWND window_)
{
    if (auto it{ renderTargets.find(window_) }; it != renderTargets.end())
    {
        WaitForGpu(*(it->second));
        if (it->second->fenceEvent)
        {
            CloseHandle(it->second->fenceEvent);
        }
        renderTargets.erase(it);
    }
}

void RenderService::BeginFrame()
{
    commandAllocators[globalFrameIndex]->Reset();
    commandList->Reset(commandAllocators[globalFrameIndex].Get(), nullptr);
}

void RenderService::Render(HWND window_, const RenderContext& renderContext_)
{
    (void)renderContext_;

    auto it{ renderTargets.find(window_) };
    if (it == renderTargets.end())
    {
        return;
    }

    RenderTarget& target{ *it->second };
    UINT currentFrameIndex{ target.frameIndex };

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = target.backBuffers[currentFrameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    D3D12_VIEWPORT viewport{ 0.0f, 0.0f, static_cast<float>(target.width), static_cast<float>(target.height), 0.0f, 1.0f };
    D3D12_RECT scissorRect{ 0, 0, target.width, target.height };
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    commandList->OMSetRenderTargets(1, &target.rtvHandles[currentFrameIndex], FALSE, &target.dsvHandle);

    const float clearColor[] = { 0.1f, 0.2f, 0.3f, 1.0f };
    commandList->ClearRenderTargetView(target.rtvHandles[currentFrameIndex], clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(target.dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // RenderContext는 컴포넌트가 제출한 RenderRequest 묶음입니다.
    // 실제 Mesh/Material 파이프라인이 들어오면 여기서 요청을 소비합니다.

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList->ResourceBarrier(1, &barrier);
}

void RenderService::EndFrame()
{
    commandList->Close();
    ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    bool vsync = GetEngine().GetOption(L"Render.VSync", true);
    for (auto& [hwnd, target] : renderTargets)
    {
        target->swapChain->Present(vsync ? 1 : 0, 0);

        const UINT64 currentFenceValue = target->fenceValues[target->frameIndex];
        commandQueue->Signal(target->fence.Get(), currentFenceValue);

        target->frameIndex = target->swapChain->GetCurrentBackBufferIndex();

        if (target->fence->GetCompletedValue() < target->fenceValues[target->frameIndex])
        {
            target->fence->SetEventOnCompletion(target->fenceValues[target->frameIndex], target->fenceEvent);
            ::WaitForSingleObjectEx(target->fenceEvent, INFINITE, FALSE);
        }

        target->fenceValues[target->frameIndex] = currentFenceValue + 1;
    }

    globalFrameIndex = (globalFrameIndex + 1) % 2;
}

void RenderService::OnAdd()
{
    if (!CreateDevice())
    {
        return;
    }
    if (!CreateCommandObjects())
    {
        return;
    }
    if (!CreateDescriptorHeaps())
    {
        return;
    }

    for (UINT i{ 0 }; i < 2; ++i)
    {
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]));
    }
    device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandAllocators[0].Get(),
        nullptr,
        IID_PPV_ARGS(&commandList)
    );
    commandList->Close();
}

void RenderService::OnRemove()
{
    for (auto& [hwnd, renderTarget] : renderTargets)
    {
        WaitForGpu(*renderTarget);
    }
    renderTargets.clear();

    for (UINT i{ 0 }; i < 2; ++i)
    {
        commandAllocators[i].Reset();
    }

    commandList.Reset();
    rtvHeap.Reset();
    dsvHeap.Reset();
    commandQueue.Reset();
    device.Reset();
}

bool RenderService::CreateDevice()
{
#if defined(_DEBUG)
    Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }
#endif

    if (FAILED(::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
    {
        return false;
    }

    return true;
}

bool RenderService::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
    {
        return false;
    }

    return true;
}

bool RenderService::CreateDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = 64;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap))))
    {
        return false;
    }
    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 32;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    if (FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap))))
    {
        return false;
    }
    dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    return true;
}

void RenderService::WaitForGpu(RenderTarget& context_)
{
    commandQueue->Signal(context_.fence.Get(), context_.fenceValues[context_.frameIndex]);
    context_.fence->SetEventOnCompletion(context_.fenceValues[context_.frameIndex], context_.fenceEvent);
    ::WaitForSingleObjectEx(context_.fenceEvent, INFINITE, FALSE);
    context_.fenceValues[context_.frameIndex]++;
}
