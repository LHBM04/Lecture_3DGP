#include "Precompiled.hpp"

#include "GPUDevice.hpp"

#include <format>
#include <limits>
#include <utility>
#include <vector>

#include "../Platform/Display.hpp"

#include "GPUBuffer.hpp"
#include "GPUCommandBuffer.hpp"
#include "GPUFence.hpp"
#include "GPUGraphicsPipelineState.hpp"
#include "GPUMesh.hpp"
#include "GPUSwapChain.hpp"
#include "GPUTarget.hpp"
#include "GPUTexture2D.hpp"

namespace TUK::Framework
{
	std::expected<GPUBuffer, std::string> GPUDevice::CreateBuffer(
		D3D12_HEAP_TYPE heapType,
		u64 sizeInBytes,
		D3D12_RESOURCE_STATES initialState,
		D3D12_RESOURCE_FLAGS flags) const
	{
		if (sizeInBytes == 0 || sizeInBytes > std::numeric_limits<usize>::max())
		{
			return std::unexpected("GPU 버퍼 크기가 유효하지 않습니다.");
		}

		const CD3DX12_HEAP_PROPERTIES heapProperties(heapType);
		const auto description = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes, flags);

		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		if (FAILED(GetD3DDevice().CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&description,
			initialState,
			nullptr,
			IID_PPV_ARGS(resource.GetAddressOf()))))
		{
			return std::unexpected("GPU 버퍼 리소스 생성에 실패했습니다.");
		}

		return GPUBuffer(std::move(resource), heapType);
	}

	std::expected<GPUBuffer, std::string> GPUDevice::CreateUploadBuffer(
		std::span<const std::byte> data) const
	{
		auto buffer = CreateBuffer(D3D12_HEAP_TYPE_UPLOAD, data.size(), D3D12_RESOURCE_STATE_GENERIC_READ);
		if (!buffer)
		{
			return std::unexpected(buffer.error());
		}

		if (auto result = buffer->Write(std::span<const u8>(reinterpret_cast<const u8*>(data.data()), data.size())); !result)
		{
			return std::unexpected(result.error());
		}

		return buffer;
	}

	std::expected<GPUCommandBuffer, std::string> GPUDevice::CreateCommandBuffer() const
	{
		ID3D12Device& device = GetD3DDevice();

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
		if (FAILED(device.CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(d3dCommandAllocator.GetAddressOf()))))
		{
			return std::unexpected("D3D12 명령 할당자 생성에 실패했습니다.");
		}

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		if (FAILED(device.CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			d3dCommandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(d3dCommandList.GetAddressOf()))))
		{
			return std::unexpected("D3D12 명령 리스트 생성에 실패했습니다.");
		}

		if (FAILED(d3dCommandList->Close()))
		{
			return std::unexpected("초기 D3D12 명령 리스트 닫기에 실패했습니다.");
		}

		return GPUCommandBuffer(*this, std::move(d3dCommandList), std::move(d3dCommandAllocator));
	}

	std::expected<GPUFence, std::string> GPUDevice::CreateFence() const
	{
		Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence;
		if (FAILED(GetD3DDevice().CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(d3dFence.GetAddressOf()))))
		{
			return std::unexpected("D3D12 펜스 생성에 실패했습니다.");
		}

		return GPUFence(std::move(d3dFence));
	}

	std::expected<GPUSwapChain, std::string> GPUDevice::CreateSwapChain(
		const Display* display,
		const DXGI_SWAP_CHAIN_DESC1& description) const
	{
		ASSERT_FMT(display, "스왑체인을 생성할 디스플레이가 nullptr입니다.");

		IDXGIFactory6& factory = GetDXGIFactory();
		ID3D12Device& device = GetD3DDevice();
		ID3D12CommandQueue& commandQueue = GetD3DCommandQueue();

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		if (FAILED(factory.CreateSwapChainForHwnd(
			&commandQueue,
			display->GetHWND(),
			&description,
			nullptr,
			nullptr,
			swapChain1.GetAddressOf())))
		{
			return std::unexpected("DXGI 스왑체인 생성에 실패했습니다.");
		}

		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain4;
		if (FAILED(swapChain1.As(&swapChain4)))
		{
			return std::unexpected("IDXGISwapChain4 인터페이스 조회에 실패했습니다.");
		}

		D3D12_DESCRIPTOR_HEAP_DESC heapDescription{};
		heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDescription.NumDescriptors = description.BufferCount;
		heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDescription.NodeMask = 0;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		if (FAILED(device.CreateDescriptorHeap(&heapDescription, IID_PPV_ARGS(rtvHeap.GetAddressOf()))))
		{
			return std::unexpected("RTV 디스크립터 힙 생성에 실패했습니다.");
		}

		const UINT descriptorSize = device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

		std::vector<GPUTexture2D> backBuffers;
		backBuffers.reserve(description.BufferCount);

		for (u32 index = 0; index < description.BufferCount; ++index)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			if (FAILED(swapChain4->GetBuffer(index, IID_PPV_ARGS(backBuffer.GetAddressOf()))))
			{
				return std::unexpected("D3D12 백 버퍼 조회에 실패했습니다.");
			}

			device.CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, descriptorSize);

			backBuffers.emplace_back(std::move(backBuffer));
		}

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDescription{};
		dsvHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDescription.NumDescriptors = 1;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
		if (FAILED(device.CreateDescriptorHeap(&dsvHeapDescription, IID_PPV_ARGS(dsvHeap.GetAddressOf()))))
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

		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil;
		if (FAILED(device.CreateCommittedResource(
			&depthHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthDescription,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(depthStencil.GetAddressOf()))))
		{
			return std::unexpected("깊이 버퍼 생성에 실패했습니다.");
		}

		device.CreateDepthStencilView(depthStencil.Get(), nullptr, dsvHeap->GetCPUDescriptorHandleForHeapStart());

		return GPUSwapChain(
			std::move(swapChain4),
			std::move(rtvHeap),
			std::move(dsvHeap),
			std::move(depthStencil),
			std::move(backBuffers),
			descriptorSize);
	}

	std::expected<GPUGraphicsPipelineState, std::string> GPUDevice::CreateGraphicsPipelineState(
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDescription = description;
		pipelineDescription.pRootSignature = &GetD3DRootSignature();

		Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState;
		const HRESULT result = GetD3DDevice().CreateGraphicsPipelineState(
			&pipelineDescription,
			IID_PPV_ARGS(d3dPipelineState.GetAddressOf()));
		if (FAILED(result))
		{
			return std::unexpected(std::format(
				"그래픽스 파이프라인 상태 생성에 실패했습니다. HRESULT: 0x{:08X}",
				static_cast<u32>(result)));
		}

		return GPUGraphicsPipelineState(std::move(d3dPipelineState));
	}

	std::expected<GPUMesh, std::string> GPUDevice::CreateMesh(
		GPUBuffer vertexBuffer,
		u32 vertexStride,
		GPUBuffer indexBuffer,
		DXGI_FORMAT indexFormat,
		D3D12_PRIMITIVE_TOPOLOGY topology) const
	{
		if (vertexStride == 0 || vertexBuffer.GetSizeInBytes() == 0
			|| vertexBuffer.GetSizeInBytes() > std::numeric_limits<u32>::max()
			|| vertexBuffer.GetSizeInBytes() % vertexStride != 0)
		{
			return std::unexpected("정점 버퍼 크기 또는 스트라이드가 유효하지 않습니다.");
		}

		if (indexFormat != DXGI_FORMAT_R16_UINT && indexFormat != DXGI_FORMAT_R32_UINT)
		{
			return std::unexpected("인덱스 형식은 R16_UINT 또는 R32_UINT여야 합니다.");
		}

		const u32 INDEX_SIZE = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
		if (indexBuffer.GetSizeInBytes() == 0
			|| indexBuffer.GetSizeInBytes() > std::numeric_limits<u32>::max()
			|| indexBuffer.GetSizeInBytes() % INDEX_SIZE != 0)
		{
			return std::unexpected("인덱스 버퍼 크기가 유효하지 않습니다.");
		}

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = vertexBuffer.GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer.GetSizeInBytes());
		vertexBufferView.StrideInBytes = vertexStride;

		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		indexBufferView.BufferLocation = indexBuffer.GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = static_cast<UINT>(indexBuffer.GetSizeInBytes());
		indexBufferView.Format = indexFormat;

		return GPUMesh(
			std::move(vertexBuffer),
			vertexBufferView,
			std::move(indexBuffer),
			indexBufferView,
			topology);
	}

	std::expected<GPUMesh, std::string> GPUDevice::CreateMesh(
		std::span<const std::byte> vertexData,
		u32 vertexStride,
		std::span<const std::byte> indexData,
		DXGI_FORMAT indexFormat,
		D3D12_PRIMITIVE_TOPOLOGY topology) const
	{
		auto vertexBuffer = CreateUploadBuffer(vertexData);
		auto indexBuffer = CreateUploadBuffer(indexData);
		if (!vertexBuffer || !indexBuffer)
		{
			return std::unexpected("메시의 GPU 버퍼 생성에 실패했습니다.");
		}

		return CreateMesh(
			std::move(*vertexBuffer),
			vertexStride,
			std::move(*indexBuffer),
			indexFormat,
			topology);
	}
}
