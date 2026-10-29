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
		return static_cast<std::uint32_t>(vertexBuffer.GetSizeInBytes() / vertexStride);
	}

	std::uint32_t GPUMesh::GetIndexCount() const noexcept
	{
		const std::uint32_t indexSize = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
		return static_cast<std::uint32_t>(indexBuffer.GetSizeInBytes() / indexSize);
	}

	D3D_PRIMITIVE_TOPOLOGY GPUMesh::GetD3DTopology() const noexcept
	{
		return static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology);
	}

	D3D12_VERTEX_BUFFER_VIEW GPUMesh::GetD3DVertexBufferView() const noexcept
	{
		D3D12_VERTEX_BUFFER_VIEW view{};
		view.BufferLocation = vertexBuffer.GetGPUVirtualAddress();
		view.SizeInBytes = static_cast<UINT>(vertexBuffer.GetSizeInBytes());
		view.StrideInBytes = vertexStride;
		return view;
	}

	D3D12_INDEX_BUFFER_VIEW GPUMesh::GetD3DIndexBufferView() const noexcept
	{
		D3D12_INDEX_BUFFER_VIEW view{};
		view.BufferLocation = indexBuffer.GetGPUVirtualAddress();
		view.SizeInBytes = static_cast<UINT>(indexBuffer.GetSizeInBytes());
		view.Format = indexFormat;
		return view;
	}

	GPUMesh::GPUMesh(
		GPUBuffer vertexBuffer,
		GPUBuffer indexBuffer,
		const GPUMeshCreateInfo& info) noexcept
		: vertexBuffer(std::move(vertexBuffer))
		, vertexStride(info.vertexStride)
		, indexBuffer(std::move(indexBuffer))
		, indexFormat(info.indexFormat)
		, topology(info.topology)
	{
	}
}
