#pragma once

#include <utility>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

#include "GPUBuffer.hpp"

namespace TUK::Framework
{
	struct GPUMeshProperties final
	{
		GPUBuffer vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		GPUBuffer indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		D3D12_PRIMITIVE_TOPOLOGY topology;
	};

	class GPUMesh
	{
	public:
		explicit GPUMesh(GPUMeshProperties&& properties) noexcept;
		~GPUMesh() noexcept = default;

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

	private:
		GPUBuffer vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

		GPUBuffer indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;

		D3D12_PRIMITIVE_TOPOLOGY topology;
	};

	inline GPUMesh::GPUMesh(GPUMeshProperties&& properties) noexcept
		: vertexBuffer(std::move(properties.vertexBuffer))
		, vertexBufferView(properties.vertexBufferView)
		, indexBuffer(std::move(properties.indexBuffer))
		, indexBufferView(properties.indexBufferView)
		, topology(properties.topology)
	{
	}
}
