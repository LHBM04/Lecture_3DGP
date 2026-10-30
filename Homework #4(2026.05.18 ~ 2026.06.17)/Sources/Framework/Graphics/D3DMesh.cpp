#include "Precompiled.hpp"
#include "D3DMesh.hpp"

namespace TUK::Framework
{
	const D3DBuffer& D3DMesh::GetVertexBuffer() const noexcept
	{
		return vertexBuffer;
	}

	const D3DBuffer& D3DMesh::GetIndexBuffer() const noexcept
	{
		return indexBuffer;
	}

	std::uint32_t D3DMesh::GetVertexCount() const noexcept
	{
		return static_cast<std::uint32_t>(vertexBufferView.SizeInBytes / vertexBufferView.StrideInBytes);
	}

	std::uint32_t D3DMesh::GetIndexCount() const noexcept
	{
		const std::uint32_t indexSize = indexBufferView.Format == DXGI_FORMAT_R16_UINT ? 2 : 4;
		return static_cast<std::uint32_t>(indexBufferView.SizeInBytes / indexSize);
	}

	D3D12_PRIMITIVE_TOPOLOGY D3DMesh::GetD3DTopology() const noexcept
	{
		return topology;
	}

	D3D12_VERTEX_BUFFER_VIEW D3DMesh::GetD3DVertexBufferView() const noexcept
	{
		return vertexBufferView;
	}

	D3D12_INDEX_BUFFER_VIEW D3DMesh::GetD3DIndexBufferView() const noexcept
	{
		return indexBufferView;
	}

	D3DMesh::D3DMesh(
		D3DBuffer vertexBuffer,
		D3DBuffer indexBuffer,
		std::uint32_t vertexStride, DXGI_FORMAT indexFormat, D3D12_PRIMITIVE_TOPOLOGY topology) noexcept
		: vertexBuffer(std::move(vertexBuffer))
		, vertexBufferView{ this->vertexBuffer.GetGPUVirtualAddress(), static_cast<UINT>(this->vertexBuffer.GetSizeInBytes()), vertexStride }
		, indexBuffer(std::move(indexBuffer))
		, indexBufferView{ this->indexBuffer.GetGPUVirtualAddress(), static_cast<UINT>(this->indexBuffer.GetSizeInBytes()), indexFormat }
		, topology(topology)
	{
	}
}
