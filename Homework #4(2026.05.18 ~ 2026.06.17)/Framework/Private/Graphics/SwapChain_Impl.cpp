#include "Precompiled.hpp"
#include "SwapChain_Impl.hpp"

#include "Framework/Graphics/CommandBuffer.hpp"

namespace TUK::Framework
{
	SwapChain_Impl::SwapChain_Impl(IDXGISwapChain4* dxgiSwapChain) noexcept
		: dxgiSwapChain(dxgiSwapChain)
		, rtvHeap(nullptr)
		, backBuffers()
		, rtvDescriptorSize(0)
	{
		assert(dxgiSwapChain);

		DXGI_SWAP_CHAIN_DESC1 description{};
		dxgiSwapChain->GetDesc1(&description);

		ID3D12Device* d3dDevice = nullptr;
		if (SUCCEEDED(dxgiSwapChain->GetDevice(IID_PPV_ARGS(&d3dDevice))) && d3dDevice)
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDesc.NumDescriptors = description.BufferCount;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			if (SUCCEEDED(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap))))
			{
				rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				backBuffers.resize(description.BufferCount, nullptr);
				CreateRenderTargets();
			}

			d3dDevice->Release();
		}
	}

	SwapChain_Impl::~SwapChain_Impl() noexcept
	{
		ReleaseRenderTargets();

		if (rtvHeap)
		{
			rtvHeap->Release();
			rtvHeap = nullptr;
		}

		if (dxgiSwapChain)
		{
			dxgiSwapChain->SetFullscreenState(FALSE, nullptr);
			dxgiSwapChain->Release();
			dxgiSwapChain = nullptr;
		}
	}

	void SwapChain_Impl::Clear(CommandBuffer& commandBuffer) const
	{
		assert(dxgiSwapChain);
		commandBuffer.ClearRenderTarget(ColorRGBA<float>(0, 0, 0, 0));
	}

	void SwapChain_Impl::Present() const
	{
		assert(dxgiSwapChain);
		dxgiSwapChain->Present(1, 0);
	}

	Vector2D<int> SwapChain_Impl::GetSize() const noexcept
	{
		assert(dxgiSwapChain);
		DXGI_SWAP_CHAIN_DESC1 description{};
		dxgiSwapChain->GetDesc1(&description);
		return { static_cast<int>(description.Width), static_cast<int>(description.Height) };
	}

	void SwapChain_Impl::SetSize(const Vector2D<int>& size) noexcept
	{
		assert(dxgiSwapChain);
		if (size.GetX() <= 0 || size.GetY() <= 0)
		{
			return;
		}

		DXGI_SWAP_CHAIN_DESC1 description{};
		if (FAILED(dxgiSwapChain->GetDesc1(&description)))
		{
			return;
		}

		ReleaseRenderTargets();

		if (SUCCEEDED(dxgiSwapChain->ResizeBuffers(
			description.BufferCount,
			static_cast<UINT>(size.GetX()),
			static_cast<UINT>(size.GetY()),
			description.Format,
			description.Flags)))
		{
			CreateRenderTargets();
		}
	}

	IDXGISwapChain4& SwapChain_Impl::GetDXGISwapChain() const noexcept
	{
		assert(dxgiSwapChain);
		return *dxgiSwapChain;
	}

	void SwapChain_Impl::CreateRenderTargets()
	{
		assert(dxgiSwapChain && rtvHeap);

		ID3D12Device* d3dDevice = nullptr;
		if (FAILED(dxgiSwapChain->GetDevice(IID_PPV_ARGS(&d3dDevice))) || !d3dDevice)
		{
			return;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT index = 0; index < static_cast<UINT>(backBuffers.size()); ++index)
		{
			if (SUCCEEDED(dxgiSwapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffers[index]))))
			{
				d3dDevice->CreateRenderTargetView(backBuffers[index], nullptr, rtvHandle);
			}
			rtvHandle.ptr += rtvDescriptorSize;
		}

		d3dDevice->Release();
	}

	void SwapChain_Impl::ReleaseRenderTargets() noexcept
	{
		for (ID3D12Resource*& backBuffer : backBuffers)
		{
			if (backBuffer)
			{
				backBuffer->Release();
				backBuffer = nullptr;
			}
		}
	}
}
