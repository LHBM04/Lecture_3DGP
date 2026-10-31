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

	u32 GPUMesh::GetVertexCount() const noexcept
	{
		return static_cast<u32>(vertexBufferView.SizeInBytes / vertexBufferView.StrideInBytes);
	}

	u32 GPUMesh::GetIndexCount() const noexcept
	{
		const u32 indexSize = indexBufferView.Format == DXGI_FORMAT_R16_UINT ? 2 : 4;
		return static_cast<u32>(indexBufferView.SizeInBytes / indexSize);
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

}
