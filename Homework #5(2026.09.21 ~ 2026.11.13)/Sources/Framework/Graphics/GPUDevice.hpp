#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <utility>

#include <wrl/client.h>

#include <directx/d3d12.h>
#include <dxgi1_6.h>

#include "../Core/Common.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	class Display;
	class GPUBuffer;
	class GPUCommandBuffer;
	class GPUFence;
	class GPUGraphicsPipelineState;
	class GPUMesh;
	class GPUSwapChain;

	class GPUDevice
	{
	public:
		explicit GPUDevice(
			Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory,
			Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice,
			Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue,
			Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature) noexcept;

		GPUDevice(const GPUDevice&) = delete;
		GPUDevice& operator=(const GPUDevice&) = delete;

		GPUDevice(GPUDevice&&) noexcept = default;
		GPUDevice& operator=(GPUDevice&&) noexcept = default;

		[[nodiscard]] std::expected<GPUBuffer, std::string> CreateBuffer(
			D3D12_HEAP_TYPE heapType,
			u64 sizeInBytes,
			D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) const;
		[[nodiscard]] std::expected<GPUBuffer, std::string> CreateUploadBuffer(
			std::span<const std::byte> data) const;
		[[nodiscard]] std::expected<GPUCommandBuffer, std::string> CreateCommandBuffer() const;
		[[nodiscard]] std::expected<GPUFence, std::string> CreateFence() const;
		[[nodiscard]] std::expected<GPUSwapChain, std::string> CreateSwapChain(
			const Display* display,
			const DXGI_SWAP_CHAIN_DESC1& description) const;
		[[nodiscard]] std::expected<GPUGraphicsPipelineState, std::string> CreateGraphicsPipelineState(
			const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const;

		[[nodiscard]] std::expected<GPUMesh, std::string> CreateMesh(
			GPUBuffer vertexBuffer,
			u32 vertexStride,
			GPUBuffer indexBuffer,
			DXGI_FORMAT indexFormat,
			D3D12_PRIMITIVE_TOPOLOGY topology) const;
		[[nodiscard]] std::expected<GPUMesh, std::string> CreateMesh(
			std::span<const std::byte> vertexData,
			u32 vertexStride,
			std::span<const std::byte> indexData,
			DXGI_FORMAT indexFormat,
			D3D12_PRIMITIVE_TOPOLOGY topology) const;

		[[nodiscard]] IDXGIFactory6& GetDXGIFactory() const noexcept;
		[[nodiscard]] ID3D12Device& GetD3DDevice() const noexcept;
		[[nodiscard]] ID3D12CommandQueue& GetD3DCommandQueue() const noexcept;
		[[nodiscard]] ID3D12RootSignature& GetD3DRootSignature() const noexcept;

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature;
	};

	inline GPUDevice::GPUDevice(
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory,
		Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue,
		Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature) noexcept
		: dxgiFactory(std::move(dxgiFactory))
		, d3dDevice(std::move(d3dDevice))
		, d3dCommandQueue(std::move(d3dCommandQueue))
		, d3dRootSignature(std::move(d3dRootSignature))
	{
		ASSERT_FMT(this->dxgiFactory.Get(), "dxgiFactory가 nullptr입니다.");
		ASSERT_FMT(this->d3dDevice.Get(), "d3dDevice가 nullptr입니다.");
		ASSERT_FMT(this->d3dCommandQueue.Get(), "d3dCommandQueue가 nullptr입니다.");
		ASSERT_FMT(this->d3dRootSignature.Get(), "d3dRootSignature가 nullptr입니다.");
	}

	inline IDXGIFactory6& GPUDevice::GetDXGIFactory() const noexcept
	{
		ASSERT_FMT(dxgiFactory.Get(), "dxgiFactory가 nullptr입니다.");

		return *dxgiFactory.Get();
	}

	inline ID3D12Device& GPUDevice::GetD3DDevice() const noexcept
	{
		ASSERT_FMT(d3dDevice.Get(), "d3dDevice가 nullptr입니다.");

		return *d3dDevice.Get();
	}

	inline ID3D12CommandQueue& GPUDevice::GetD3DCommandQueue() const noexcept
	{
		ASSERT_FMT(d3dCommandQueue.Get(), "d3dCommandQueue가 nullptr입니다.");

		return *d3dCommandQueue.Get();
	}

	inline ID3D12RootSignature& GPUDevice::GetD3DRootSignature() const noexcept
	{
		ASSERT_FMT(d3dRootSignature.Get(), "d3dRootSignature가 nullptr입니다.");

		return *d3dRootSignature.Get();
	}
}
