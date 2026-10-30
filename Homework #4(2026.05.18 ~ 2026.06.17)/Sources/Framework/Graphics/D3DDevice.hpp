#pragma once

#include <expected>
#include <string>
#include <string_view>

#include <d3d12.h>

#include <wrl/client.h>

#include "D3DBuffer.hpp"
#include "D3DCommandBuffer.hpp"
#include "D3DFence.hpp"
#include "D3DGraphicsPipelineState.hpp"
#include "D3DMesh.hpp"
#include "D3DRootSignature.hpp"
#include "D3DShader.hpp"

namespace TUK::Framework
{
	class D3DDevice
	{
		friend class RenderSubsystem;

	public:
		~D3DDevice() noexcept = default;

		D3DDevice(const D3DDevice&) = delete;
		D3DDevice& operator=(const D3DDevice&) = delete;

		D3DDevice(D3DDevice&&) noexcept = default;
		D3DDevice& operator=(D3DDevice&&) noexcept = default;

		[[nodiscard]] std::expected<D3DBuffer, std::string> CreateD3DBuffer(const D3D12_HEAP_PROPERTIES& heapProperties, const D3D12_RESOURCE_DESC& description, D3D12_RESOURCE_STATES initialState) const;

		[[nodiscard]] std::expected<D3DMesh, std::string> CreateD3DMesh(
			D3DBuffer vertexBuffer,
			std::uint32_t vertextStride,
			D3DBuffer indexBuffer,
			DXGI_FORMAT indexFormat,
			D3D12_PRIMITIVE_TOPOLOGY topology) const;

		[[nodiscard]] std::expected<D3DShader, std::string> CreateD3DShader(const std::filesystem::path& path, std::string_view target, const std::string& entryPoint, std::uint32_t compileFlags) const;
		[[nodiscard]] std::expected<D3DRootSignature, std::string> CreateD3DRootSignature(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& description) const;
		[[nodiscard]] std::expected<D3DGraphicsPipelineState, std::string> CreateD3DGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const;

		[[nodiscard]] std::expected<D3DCommandBuffer, std::string> CreateD3DCommandBuffer() const;
		[[nodiscard]] std::expected<D3DFence, std::string> CreateD3DFence() const;

		[[nodiscard]] std::expected<void, std::string> SubmitCommandBuffer(D3DCommandBuffer& commandBuffer);

		[[nodiscard]] ID3D12Device& GetD3DDevice() const noexcept;
		[[nodiscard]] ID3D12CommandQueue& GetD3DCommandQueue() const noexcept;

	private:
		explicit D3DDevice(
			Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice,
			Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue) noexcept;

		Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue;
	};
}
