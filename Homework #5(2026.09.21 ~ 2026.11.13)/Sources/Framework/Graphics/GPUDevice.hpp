#pragma once

#include <expected>
#include <filesystem>
#include <string>

#include <wrl.h>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

#include "GPUCommandBuffer.hpp"
#include "GPUBuffer.hpp"
#include "GPUMesh.hpp"
#include "GPUShader.hpp"
#include "GPUFence.hpp"
#include "GPUGraphicsPipelineState.hpp"

namespace TUK::Framework
{
	struct GPUDeviceProperties final
	{
		ComPtr<ID3D12Device> d3dDevice;
		ComPtr<ID3D12CommandQueue> d3dCommandQueue;
		ComPtr<ID3D12RootSignature> d3dRootSignature;
	};

	class GPUDevice
	{
	public:
		explicit GPUDevice(const GPUDeviceProperties& properties);
		~GPUDevice() noexcept = default;

		GPUDevice(const GPUDevice&) = delete;
		GPUDevice& operator=(const GPUDevice&) = delete;

		GPUDevice(GPUDevice&&) = default;
		GPUDevice& operator=(GPUDevice&&) = default;

		[[nodiscard]] std::expected<GPUGraphicsPipelineState, std::string> CreateGPUGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const;
		[[nodiscard]] std::expected<GPUCommandBuffer, std::string> CreateGPUCommandBuffer() const;
		[[nodiscard]] std::expected<GPUBuffer, std::string> CreateGPUBuffer(
			const D3D12_HEAP_PROPERTIES& heapProperties, const D3D12_RESOURCE_DESC& description,
			D3D12_RESOURCE_STATES initialState) const;
		[[nodiscard]] std::expected<GPUMesh, std::string> CreateGPUMesh(
			GPUBuffer vertexBuffer, u32 vertexStride, GPUBuffer indexBuffer,
			DXGI_FORMAT indexFormat, D3D12_PRIMITIVE_TOPOLOGY topology) const;
		[[nodiscard]] std::expected<GPUShader, std::string> CreateGPUShader(
			const std::filesystem::path& path, LPCTSTR target,
			LPCTSTR entryPoint) const;
		[[nodiscard]] std::expected<GPUFence, std::string> CreateGPUFence() const;

		[[nodiscard]] ID3D12RootSignature& GetD3DRootSignature() const noexcept;
		[[nodiscard]] ID3D12Device& GetD3DDevice() const noexcept;
		[[nodiscard]] ID3D12CommandQueue& GetD3DCommandQueue() const noexcept;

	private:
		ComPtr<ID3D12Device> d3dDevice;
		ComPtr<ID3D12CommandQueue> d3dCommandQueue;
		ComPtr<ID3D12RootSignature> d3dRootSignature;
	};

	inline GPUDevice::GPUDevice(const GPUDeviceProperties& properties)
		: d3dDevice(properties.d3dDevice)
		, d3dCommandQueue(properties.d3dCommandQueue)
		, d3dRootSignature(properties.d3dRootSignature)
	{
	}

	inline ID3D12RootSignature& GPUDevice::GetD3DRootSignature() const noexcept
	{
		return *d3dRootSignature.Get();
	}

	inline ID3D12Device& GPUDevice::GetD3DDevice() const noexcept
	{
		return *d3dDevice.Get();
	}

	inline ID3D12CommandQueue& GPUDevice::GetD3DCommandQueue() const noexcept
	{
		return *d3dCommandQueue.Get();
	}
}
