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

		ID3D12CommandAllocator* d3dCommandAllocator = nullptr;
		if (FAILED(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3dCommandAllocator))))
		{
			return std::unexpected("D3D12 Command Allocator 생성에 실패했습니다.");
		}

		ID3D12GraphicsCommandList10* d3dCommandList = nullptr;
		if (FAILED(d3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			d3dCommandAllocator,
			nullptr,
			IID_PPV_ARGS(&d3dCommandList))))
		{
			d3dCommandAllocator->Release();
			return std::unexpected("D3D12 Command List 생성에 실패했습니다.");
		}

		d3dCommandList->Close();

		return GPUCommandBuffer(d3dCommandList, d3dCommandAllocator);
	}

	std::expected<GPUFence, std::string> GPUDevice::CreateGPUFence() const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}
		
		ID3D12Fence* d3dFence = nullptr;
		if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3dFence))))
		{
			return std::unexpected("D3D12 Fence 생성에 실패했습니다.");
		}

		return GPUFence(d3dFence);
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
		ID3D12Device* d3dDevice,
		ID3D12CommandQueue* d3dCommandQueue) noexcept
		: d3dDevice(d3dDevice)
		, d3dCommandQueue(d3dCommandQueue)
	{
	}
}
