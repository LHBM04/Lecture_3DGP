#include "Precompiled.h"
#include "SwapChain.h"

#include "DescriptorAllocator.h"

bool SwapChain::Initialize(const SwapChainOptions& options_)
{
	if (options_.factory == nullptr ||
		options_.device == nullptr ||
		options_.commandQueue == nullptr ||
		options_.window == nullptr ||
		options_.backBufferCount == 0 ||
		options_.rtvAllocator == nullptr ||
		options_.dsvAllocator == nullptr)
	{
		return false;
	}

	RECT clientRect{};
	if (!::GetClientRect(options_.window, &clientRect))
	{
		return false;
	}

	clientWidth = static_cast<UINT>(clientRect.right - clientRect.left);
	clientHeight = static_cast<UINT>(clientRect.bottom - clientRect.top);
	if (clientWidth == 0 || clientHeight == 0)
	{
		return false;
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = clientWidth;
	swapChainDesc.Height = clientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = options_.backBufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> newSwapChain;
	if (FAILED(options_.factory->CreateSwapChainForHwnd(
		options_.commandQueue,
		options_.window,
		&swapChainDesc,
		nullptr,
		nullptr,
		newSwapChain.GetAddressOf())))
	{
		return false;
	}

	options_.factory->MakeWindowAssociation(options_.window, DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(newSwapChain.As(&swapChain)))
	{
		return false;
	}

	backBuffers.resize(options_.backBufferCount);
	rtvHandles.resize(options_.backBufferCount);
	for (UINT index{ 0 }; index < options_.backBufferCount; ++index)
	{
		if (FAILED(swapChain->GetBuffer(index, IID_PPV_ARGS(backBuffers[index].GetAddressOf()))))
		{
			return false;
		}

		rtvHandles[index] = options_.rtvAllocator->GetCpuHandle(index);
		options_.device->CreateRenderTargetView(backBuffers[index].Get(), nullptr, rtvHandles[index]);
	}

	D3D12_HEAP_PROPERTIES depthHeapProperties{};
	depthHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	depthHeapProperties.CreationNodeMask = 1;
	depthHeapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC depthStencilDesc{};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = clientWidth;
	depthStencilDesc.Height = clientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.DepthStencil.Stencil = 0;

	if (FAILED(options_.device->CreateCommittedResource(
		&depthHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(depthStencilBuffer.GetAddressOf()))))
	{
		return false;
	}

	dsvHandle = options_.dsvAllocator->GetCpuHandle(0);
	options_.device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, dsvHandle);

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(clientWidth);
	viewport.Height = static_cast<float>(clientHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = static_cast<LONG>(clientWidth);
	scissorRect.bottom = static_cast<LONG>(clientHeight);

	return true;
}

void SwapChain::Release()
{
	depthStencilBuffer.Reset();
	backBuffers.clear();
	rtvHandles.clear();
	swapChain.Reset();
	dsvHandle = {};
	viewport = {};
	scissorRect = {};
	clientWidth = 0;
	clientHeight = 0;
}

IDXGISwapChain3& SwapChain::GetDXGISwapChain() const noexcept
{
	assert(swapChain != nullptr);
	return *swapChain.Get();
}

ID3D12Resource& SwapChain::GetCurrentBackBuffer(UINT frameIndex_) const noexcept
{
	assert(backBuffers[frameIndex_] != nullptr);
	return *backBuffers[frameIndex_].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCurrentRtvHandle(UINT frameIndex_) const noexcept
{
	return rtvHandles[frameIndex_];
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetDsvHandle() const noexcept
{
	return dsvHandle;
}

const D3D12_VIEWPORT& SwapChain::GetViewport() const noexcept
{
	return viewport;
}

const D3D12_RECT& SwapChain::GetScissorRect() const noexcept
{
	return scissorRect;
}
