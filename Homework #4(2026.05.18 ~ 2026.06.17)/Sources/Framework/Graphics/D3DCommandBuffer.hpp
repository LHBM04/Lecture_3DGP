#pragma once

#include <cstdint>

#include <d3d12.h>

#include <wrl/client.h>

#include "../Math/ColorRGBA.hpp"

namespace TUK::Framework
{
	class D3DBuffer;
	class D3DGraphicsPipelineState;
	class D3DMesh;
	class D3DRootSignature;
	class D3DTarget;
	class D3DTexture;

	class D3DCommandBuffer
	{
		friend class D3DDevice;

	public:
		~D3DCommandBuffer() noexcept = default;

		D3DCommandBuffer(const D3DCommandBuffer&) = delete;
		D3DCommandBuffer& operator=(const D3DCommandBuffer&) = delete;

		D3DCommandBuffer(D3DCommandBuffer&&) noexcept = default;
		D3DCommandBuffer& operator=(D3DCommandBuffer&&) noexcept = default;

		void TransitionTexture(
			const D3DTexture& texture,
			D3D12_RESOURCE_STATES stateBefore,
			D3D12_RESOURCE_STATES stateAfter);

		void TransitionBuffer(
			const D3DBuffer& buffer,
			D3D12_RESOURCE_STATES stateBefore,
			D3D12_RESOURCE_STATES stateAfter);

		void CopyD3DBuffer(
			const D3DBuffer& destination,
			std::uint64_t destinationOffset,
			const D3DBuffer& source,
			std::uint64_t sourceOffset,
			std::uint64_t sizeInBytes);

		void SetGPUGraphicsRootSignature(const D3DRootSignature& rootSignature);
		void SetD3DGraphicsPipelineState(const D3DGraphicsPipelineState& pipelineState);

		void SetGPUGraphicsConstantBuffer(std::uint32_t rootParameterIndex, const D3DBuffer& buffer, std::uint64_t offset = 0);
		void SetViewport(const D3D12_VIEWPORT& viewport);
		void SetScissorRect(const D3D12_RECT& scissorRect);

		void SetD3DMesh(const D3DMesh& mesh);

		void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex = 0);
		void DrawIndexed(std::uint32_t indexCount, std::uint32_t firstIndex = 0, std::int32_t baseVertex = 0);

		void ClearD3DTarget(const D3DTarget& target, const ColorRGBA<float>& clearColor);
		void SetD3DTarget(const D3DTarget& target);

		[[nodiscard]] ID3D12GraphicsCommandList10& GetD3DCommandList() const noexcept;
		[[nodiscard]] ID3D12CommandAllocator& GetD3DCommandAllocator() const noexcept;

	private:
		explicit D3DCommandBuffer(
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList,
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator) noexcept;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
	};
}
