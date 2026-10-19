#include "Precompiled.hpp"
#include "Framework/Graphics/RenderTarget.hpp"

namespace TUK::Framework
{
	RenderTarget::RenderTarget(ID3D12Resource* d3dResource) noexcept
		: d3dResource(d3dResource)
		, d3dRtvHeap(nullptr)
	{
		assert(d3dResource);

		ID3D12Device* d3dDevice = nullptr;
		if (SUCCEEDED(d3dResource->GetDevice(IID_PPV_ARGS(&d3dDevice))) && d3dDevice)
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDesc.NumDescriptors = 1;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			if (SUCCEEDED(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&d3dRtvHeap))) && d3dRtvHeap)
			{
				d3dDevice->CreateRenderTargetView(d3dResource, nullptr, d3dRtvHeap->GetCPUDescriptorHandleForHeapStart());
			}

			d3dDevice->Release();
		}
	}

	RenderTarget::~RenderTarget() noexcept
	{
		if (d3dRtvHeap)
		{
			d3dRtvHeap->Release();
			d3dRtvHeap = nullptr;
		}

		if (d3dResource)
		{
			d3dResource->Release();
			d3dResource = nullptr;
		}
	}

	ID3D12Resource& RenderTarget::GetD3D12Resource() const noexcept
	{
		assert(d3dResource);
		return *d3dResource;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetD3D12RTVHandle() const noexcept
	{
		assert(d3dRtvHeap);
		return d3dRtvHeap->GetCPUDescriptorHandleForHeapStart();
	}
}
