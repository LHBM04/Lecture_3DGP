#include "Precompiled.hpp"
#include "GPUSwapChain.hpp"

#include "../Platform/Display.hpp"

#include "GPUDevice.hpp"

namespace TUK::Framework
{
	std::expected<GPUSwapChain, std::string> GPUSwapChain::Create(
		const GPUDevice& device,
		const Display* display,
		const DXGI_SWAP_CHAIN_DESC1& description)
	{
		if (!display)
		{
			return std::unexpected("Display is null.");
		}

		IDXGIFactory6& dxgiFactory = device.GetDXGIFactory();
		ID3D12Device& d3dDevice = device.GetD3DDevice();
		ID3D12CommandQueue& d3dCommandQueue = device.GetD3DCommandQueue();

		ComPtr<IDXGISwapChain1> swapChain1;
		if (FAILED(dxgiFactory.CreateSwapChainForHwnd(
			&d3dCommandQueue,
			display->GetHWND(),
			&description,
			nullptr,
			nullptr,
			swapChain1.GetAddressOf())))
		{
			return std::unexpected("Failed to create DXGI swap chain.");
		}

		ComPtr<IDXGISwapChain4> swapChain4;
		if (FAILED(swapChain1.As(&swapChain4)))
		{
			return std::unexpected("Failed to query IDXGISwapChain4 interface.");
		}

		D3D12_DESCRIPTOR_HEAP_DESC heapDescription{};
		heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDescription.NumDescriptors = description.BufferCount;
		heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDescription.NodeMask = 0;

		ComPtr<ID3D12DescriptorHeap> rtvHeap;
		if (FAILED(d3dDevice.CreateDescriptorHeap(&heapDescription, IID_PPV_ARGS(rtvHeap.GetAddressOf()))))
		{
			return std::unexpected("Failed to create RTV descriptor heap.");
		}

		const u32 descriptorSize = d3dDevice.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

		std::vector<GPUTexture2D> backBuffers;
		backBuffers.reserve(description.BufferCount);

		for (u32 index = 0; index < description.BufferCount; ++index)
		{
			ComPtr<ID3D12Resource> backBuffer;
			if (FAILED(swapChain4->GetBuffer(index, IID_PPV_ARGS(backBuffer.GetAddressOf()))))
			{
				return std::unexpected("Failed to retrieve D3D12 back buffer.");
			}

			d3dDevice.CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, descriptorSize);

			backBuffers.emplace_back(std::move(backBuffer));
		}

		return GPUSwapChain(
			std::move(swapChain4),
			std::move(rtvHeap),
			std::move(backBuffers),
			descriptorSize);
	}

	void GPUSwapChain::Present() const
	{
		dxgiSwapChain->Present(1, 0);
	}

	GPUTarget GPUSwapChain::GetCurrentBackBufferTarget() const noexcept
	{
		const u32 index = dxgiSwapChain->GetCurrentBackBufferIndex();
		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			static_cast<INT>(index),
			rtvDescriptorSize);

		const GPUTexture2D& texture = backBuffers[index];
		return GPUTarget(texture, rtvHandle, texture.GetD3DResource().GetDesc().Format);
	}
}
