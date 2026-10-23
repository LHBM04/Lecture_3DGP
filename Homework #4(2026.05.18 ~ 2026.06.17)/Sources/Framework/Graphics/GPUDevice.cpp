#include "Precompiled.hpp"
#include "GPUDevice.hpp"

namespace TUK::Framework
{
	std::expected<GPUCommandBuffer, std::string> GPUDevice::CreateGPUCommandBuffer() const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
		if (FAILED(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(d3dCommandAllocator.GetAddressOf()))))
		{
			return std::unexpected("D3D12 Command Allocator 생성에 실패했습니다.");
		}

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		if (FAILED(d3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			d3dCommandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(d3dCommandList.GetAddressOf()))))
		{
			return std::unexpected("D3D12 Command List 생성에 실패했습니다.");
		}

		d3dCommandList->Close();

		return GPUCommandBuffer(std::move(d3dCommandList), std::move(d3dCommandAllocator));
	}

	std::expected<GPUFence, std::string> GPUDevice::CreateGPUFence() const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}
		
		Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence;
		if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(d3dFence.GetAddressOf()))))
		{
			return std::unexpected("D3D12 Fence 생성에 실패했습니다.");
		}

		return GPUFence(std::move(d3dFence));
	}

	void GPUDevice::SubmitCommandBuffer(GPUCommandBuffer& commandBuffer)
	{
		ID3D12GraphicsCommandList10& d3dCommandList = commandBuffer.GetD3DCommandList();
		ID3D12CommandAllocator& d3dCommandAllocator = commandBuffer.GetD3DCommandAllocator();
		d3dCommandList.Close();
		ID3D12CommandList* commandLists[] = { &d3dCommandList };
		d3dCommandQueue->ExecuteCommandLists(1, commandLists);
		d3dCommandAllocator.Reset();
		d3dCommandList.Reset(&d3dCommandAllocator, nullptr);
	}

	ID3D12Device& GPUDevice::GetD3DDevice() const noexcept
	{
		return *(d3dDevice.Get());
	}

	ID3D12CommandQueue& GPUDevice::GetD3DCommandQueue() const noexcept
	{
		return *(d3dCommandQueue.Get());
	}

	GPUDevice::GPUDevice(
		Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue) noexcept
		: d3dDevice(std::move(d3dDevice))
		, d3dCommandQueue(std::move(d3dCommandQueue))
	{
	}
}
