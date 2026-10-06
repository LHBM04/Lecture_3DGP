#pragma once

#include <expected>
#include <string>

#include <cstddef>
#include <span>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <wrl.h>

#include "SwapChain.h"
#include "Fence.h"
#include "Buffer.h"
#include "GraphicsPipeline.h"
#include "ComputePipeline.h"

namespace TUK::Framework
{
	class GraphicsDevice
	{
	public:
		GraphicsDevice() noexcept;
		~GraphicsDevice() noexcept = default;

		GraphicsDevice(const GraphicsDevice&) = delete;
		GraphicsDevice& operator=(const GraphicsDevice&) = delete;
		GraphicsDevice(GraphicsDevice&&) = delete;
		GraphicsDevice& operator=(GraphicsDevice&&) = delete;

		[[nodiscard]] std::expected<void, std::string> Initialize();
		/** GPU 작업 완료 및 종속 객체 해제 후 호출한다. */
		void Release() noexcept;
		[[nodiscard]] ID3D12Device& GetNativeDevice() const noexcept;

		/** 생성한 객체의 소유권은 호출자에게 반환한다. */
		[[nodiscard]] std::expected<SwapChain, std::string> CreateSwapChain(
			Window& window, ID3D12CommandQueue& queue, UINT width, UINT height) const;
		[[nodiscard]] std::expected<Buffer, std::string> CreateBuffer(
			UINT64 size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState,
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) const;
		[[nodiscard]] std::expected<Microsoft::WRL::ComPtr<ID3D12Resource>, std::string> CreateTexture(
			const D3D12_RESOURCE_DESC& description, D3D12_RESOURCE_STATES initialState,
			const D3D12_CLEAR_VALUE* clearValue = nullptr) const;
		[[nodiscard]] std::expected<Microsoft::WRL::ComPtr<ID3D12CommandQueue>, std::string> CreateCommandQueue(
			const D3D12_COMMAND_QUEUE_DESC& description) const;
		[[nodiscard]] std::expected<Fence, std::string> CreateFence(
			UINT64 initialValue, D3D12_FENCE_FLAGS flags) const;

		[[nodiscard]] std::expected<Microsoft::WRL::ComPtr<ID3D12RootSignature>, std::string> CreateRootSignature(
			std::span<const std::byte> serializedSignature) const;
		/** description.pRootSignature에 유효한 루트 시그니처를 지정한다. */
		[[nodiscard]] std::expected<GraphicsPipeline, std::string> CreateGraphicsPipeline(
			const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const;
		[[nodiscard]] std::expected<ComputePipeline, std::string> CreateComputePipeline(
			const D3D12_COMPUTE_PIPELINE_STATE_DESC& description) const;

	private:
		[[nodiscard]] std::expected<Microsoft::WRL::ComPtr<ID3D12Resource>, std::string> CreateResource(
			const D3D12_RESOURCE_DESC& description, D3D12_HEAP_TYPE heapType,
			D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue) const;

		Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
		Microsoft::WRL::ComPtr<ID3D12Device> device;
	};
}
