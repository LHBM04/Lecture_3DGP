#include "Precompiled.hpp"
#include "GPUMesh.hpp"

namespace TUK::Framework
{
	const GPUBuffer& GPUMesh::GetVertexBuffer() const noexcept
	{
		return vertexBuffer;
	}

	const GPUBuffer& GPUMesh::GetIndexBuffer() const noexcept
	{
		return indexBuffer;
	}

	std::uint32_t GPUMesh::GetVertexCount() const noexcept
	{
		return static_cast<std::uint32_t>(vertexBufferView.SizeInBytes / vertexBufferView.StrideInBytes);
	}

	std::uint32_t GPUMesh::GetIndexCount() const noexcept
	{
		const std::uint32_t indexSize = indexBufferView.Format == DXGI_FORMAT_R16_UINT ? 2 : 4;
		return static_cast<std::uint32_t>(indexBufferView.SizeInBytes / indexSize);
	}

	D3D12_PRIMITIVE_TOPOLOGY GPUMesh::GetD3DTopology() const noexcept
	{
		return topology;
	}

	D3D12_VERTEX_BUFFER_VIEW GPUMesh::GetD3DVertexBufferView() const noexcept
	{
		return vertexBufferView;
	}

	D3D12_INDEX_BUFFER_VIEW GPUMesh::GetD3DIndexBufferView() const noexcept
	{
		return indexBufferView;
	}

	GPUMesh::GPUMesh(
		GPUBuffer vertexBuffer,
		GPUBuffer indexBuffer,
		std::uint32_t vertexStride, DXGI_FORMAT indexFormat, D3D12_PRIMITIVE_TOPOLOGY topology) noexcept
		: vertexBuffer(std::move(vertexBuffer))
		, vertexBufferView{ this->vertexBuffer.GetGPUVirtualAddress(), static_cast<UINT>(this->vertexBuffer.GetSizeInBytes()), vertexStride }
		, indexBuffer(std::move(indexBuffer))
		, indexBufferView{ this->indexBuffer.GetGPUVirtualAddress(), static_cast<UINT>(this->indexBuffer.GetSizeInBytes()), indexFormat }
		, topology(topology)
	{
	}
}
