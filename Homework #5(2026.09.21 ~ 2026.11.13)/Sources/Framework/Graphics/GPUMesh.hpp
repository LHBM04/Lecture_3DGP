#pragma once

#include <utility>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

#include "GPUBuffer.hpp"

namespace TUK::Framework
{
	struct GPUMeshCreateInfo final
	{
		GPUBuffer vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		GPUBuffer indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	};

	class GPUMesh
	{
	public:
		explicit GPUMesh(GPUMeshCreateInfo&& createInfo) noexcept;

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

	inline GPUMesh::GPUMesh(GPUMeshCreateInfo&& createInfo) noexcept
		: vertexBuffer(std::move(createInfo.vertexBuffer))
		, vertexBufferView(createInfo.vertexBufferView)
		, indexBuffer(std::move(createInfo.indexBuffer))
		, indexBufferView(createInfo.indexBufferView)
		, topology(createInfo.topology)
	{
	}
}
