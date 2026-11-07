#pragma once

#include <expected>
#include <string>
#include <utility>

#include <wrl/client.h>

#include <directx/d3d12.h>
#include <dxgi1_6.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUDevice
	{
	public:

		GPUDevice(const GPUDevice&) = delete;
		GPUDevice& operator=(const GPUDevice&) = delete;

		GPUDevice(GPUDevice&&) noexcept = default;
		GPUDevice& operator=(GPUDevice&&) noexcept = default;

		[[nodiscard]] IDXGIFactory6& GetDXGIFactory() const noexcept;
		[[nodiscard]] ID3D12Device& GetD3DDevice() const noexcept;
		[[nodiscard]] ID3D12CommandQueue& GetD3DCommandQueue() const noexcept;
		[[nodiscard]] ID3D12RootSignature& GetD3DRootSignature() const noexcept;

		[[nodiscard]] static std::expected<GPUDevice, std::string> Create();

	private:
		explicit GPUDevice(
			ComPtr<IDXGIFactory6> dxgiFactory,
			ComPtr<ID3D12Device> d3dDevice,
			ComPtr<ID3D12CommandQueue> d3dCommandQueue,
			ComPtr<ID3D12RootSignature> d3dRootSignature) noexcept;

		ComPtr<IDXGIFactory6> dxgiFactory;
		ComPtr<ID3D12Device> d3dDevice;
		ComPtr<ID3D12CommandQueue> d3dCommandQueue;
		ComPtr<ID3D12RootSignature> d3dRootSignature;
	};

	inline IDXGIFactory6& GPUDevice::GetDXGIFactory() const noexcept
	{
		return *dxgiFactory.Get();
	}

	inline ID3D12Device& GPUDevice::GetD3DDevice() const noexcept
	{
		return *d3dDevice.Get();
	}

	inline ID3D12CommandQueue& GPUDevice::GetD3DCommandQueue() const noexcept
	{
		return *d3dCommandQueue.Get();
	}

	inline ID3D12RootSignature& GPUDevice::GetD3DRootSignature() const noexcept
	{
		return *d3dRootSignature.Get();
	}

	inline GPUDevice::GPUDevice(
		ComPtr<IDXGIFactory6> dxgiFactory,
		ComPtr<ID3D12Device> d3dDevice,
		ComPtr<ID3D12CommandQueue> d3dCommandQueue,
		ComPtr<ID3D12RootSignature> d3dRootSignature) noexcept
		: dxgiFactory(std::move(dxgiFactory))
		, d3dDevice(std::move(d3dDevice))
		, d3dCommandQueue(std::move(d3dCommandQueue))
		, d3dRootSignature(std::move(d3dRootSignature))
	{
		ASSERT(this->dxgiFactory.Get() && this->d3dDevice.Get() &&
			this->d3dCommandQueue.Get() && this->d3dRootSignature.Get());
	}
}
