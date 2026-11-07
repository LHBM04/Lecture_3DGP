#pragma once

#include <expected>
#include <string>
#include <utility>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

#include "GPUBuffer.hpp"

namespace TUK::Framework
{
	class GPUMesh
	{
	public:

		GPUMesh(const GPUMesh&) = delete;
		GPUMesh& operator=(const GPUMesh&) = delete;

		GPUMesh(GPUMesh&&) noexcept = default;
		GPUMesh& operator=(GPUMesh&&) noexcept = default;

		[[nodiscard]] const GPUBuffer& GetVertexBuffer() const noexcept;
		[[nodiscard]] u32 GetVertexCount() const noexcept;
		[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetD3DVertexBufferView() const noexcept;

		[[nodiscard]] const GPUBuffer& GetIndexBuffer() const noexcept;
		[[nodiscard]] u32 GetIndexCount() const noexcept;
		[[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetD3DIndexBufferView() const noexcept;

		[[nodiscard]] D3D12_PRIMITIVE_TOPOLOGY GetD3DTopology() const noexcept;

		[[nodiscard]] static std::expected<GPUMesh, std::string> Create(
			GPUBuffer vertexBuffer,
			u32 vertexStride,
			GPUBuffer indexBuffer,
			DXGI_FORMAT indexFormat,
			D3D12_PRIMITIVE_TOPOLOGY topology);

	private:
		explicit GPUMesh(
			GPUBuffer&& vertexBuffer,
			const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
			GPUBuffer&& indexBuffer,
			const D3D12_INDEX_BUFFER_VIEW& indexBufferView,
			D3D12_PRIMITIVE_TOPOLOGY topology) noexcept;

		GPUBuffer vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

		GPUBuffer indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;

		D3D12_PRIMITIVE_TOPOLOGY topology;
	};

	inline GPUMesh::GPUMesh(
		GPUBuffer&& vertexBuffer,
		const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
		GPUBuffer&& indexBuffer,
		const D3D12_INDEX_BUFFER_VIEW& indexBufferView,
		D3D12_PRIMITIVE_TOPOLOGY topology) noexcept
		: vertexBuffer(std::move(vertexBuffer))
		, vertexBufferView(vertexBufferView)
		, indexBuffer(std::move(indexBuffer))
		, indexBufferView(indexBufferView)
		, topology(topology)
	{
	}
}
