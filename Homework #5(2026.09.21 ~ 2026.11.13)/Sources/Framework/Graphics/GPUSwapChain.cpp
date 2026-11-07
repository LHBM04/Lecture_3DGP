#include "Precompiled.hpp"
#include "GPUSwapChain.hpp"

#include "../Platform/Display.hpp"

#include "../Utilities/Logger.hpp"

#include "GPUDevice.hpp"

namespace TUK::Framework
{
	std::expected<GPUSwapChain, std::string> GPUSwapChain::Create(
		const GPUDevice& device,
		const Display* display,
		const DXGI_SWAP_CHAIN_DESC1& description)
	{
		ASSERT_FMT(display, "스왑체인을 생성할 디스플레이가 nullptr입니다.");
		if (!display)
		{
			return std::unexpected("디스플레이가 nullptr입니다.");
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
			return std::unexpected("DXGI 스왑체인 생성에 실패했습니다.");
		}

		ComPtr<IDXGISwapChain4> swapChain4;
		if (FAILED(swapChain1.As(&swapChain4)))
		{
			return std::unexpected("IDXGISwapChain4 인터페이스 조회에 실패했습니다.");
		}

		D3D12_DESCRIPTOR_HEAP_DESC heapDescription{};
		heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDescription.NumDescriptors = description.BufferCount;
		heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDescription.NodeMask = 0;

		ComPtr<ID3D12DescriptorHeap> rtvHeap;
		if (FAILED(d3dDevice.CreateDescriptorHeap(&heapDescription, IID_PPV_ARGS(rtvHeap.GetAddressOf()))))
		{
			return std::unexpected("RTV 디스크립터 힙 생성에 실패했습니다.");
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
				return std::unexpected("D3D12 백 버퍼 조회에 실패했습니다.");
			}

			d3dDevice.CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, descriptorSize);

			backBuffers.emplace_back(std::move(backBuffer));
		}

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDescription{};
		dsvHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDescription.NumDescriptors = 1;

		ComPtr<ID3D12DescriptorHeap> dsvHeap;
		if (FAILED(d3dDevice.CreateDescriptorHeap(&dsvHeapDescription, IID_PPV_ARGS(dsvHeap.GetAddressOf()))))
		{
			return std::unexpected("DSV 디스크립터 힙 생성에 실패했습니다.");
		}

		const CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
		const D3D12_RESOURCE_DESC backBufferDescription = backBuffers.front().GetD3DResource().GetDesc();
		const CD3DX12_RESOURCE_DESC depthDescription = CD3DX12_RESOURCE_DESC::Tex2D(
			GPUTarget::DEPTH_FORMAT,
			backBufferDescription.Width,
			backBufferDescription.Height,
			1,
			1,
			backBufferDescription.SampleDesc.Count,
			backBufferDescription.SampleDesc.Quality,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

		D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.Format = GPUTarget::DEPTH_FORMAT;
		depthClearValue.DepthStencil.Depth = GPUTarget::DEPTH_CLEAR_VALUE;

		ComPtr<ID3D12Resource> depthStencil;
		if (FAILED(d3dDevice.CreateCommittedResource(
			&depthHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthDescription,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(depthStencil.GetAddressOf()))))
		{
			return std::unexpected("깊이 버퍼 생성에 실패했습니다.");
		}

		d3dDevice.CreateDepthStencilView(depthStencil.Get(), nullptr, dsvHeap->GetCPUDescriptorHandleForHeapStart());

		return GPUSwapChain(
			std::move(swapChain4),
			std::move(rtvHeap),
			std::move(dsvHeap),
			std::move(depthStencil),
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
		return GPUTarget(
			texture,
			rtvHandle,
			dsvHeap->GetCPUDescriptorHandleForHeapStart(),
			texture.GetD3DResource().GetDesc().Format);
	}
}
