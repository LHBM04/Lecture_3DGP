#include "Precompiled.hpp"
#include "GPUMesh.hpp"

#include <limits>

namespace TUK::Framework
{
	std::expected<GPUMesh, std::string> GPUMesh::Create(
		GPUBuffer vertexBuffer,
		u32 vertexStride,
		GPUBuffer indexBuffer,
		DXGI_FORMAT indexFormat,
		D3D12_PRIMITIVE_TOPOLOGY topology)
	{
		if (vertexStride == 0 || vertexBuffer.GetSizeInBytes() == 0
			|| vertexBuffer.GetSizeInBytes() > std::numeric_limits<u32>::max()
			|| vertexBuffer.GetSizeInBytes() % vertexStride != 0)
		{
			return std::unexpected("Invalid vertex buffer size or stride.");
		}

		if (indexFormat != DXGI_FORMAT_R16_UINT && indexFormat != DXGI_FORMAT_R32_UINT)
		{
			return std::unexpected("Index format must be R16_UINT or R32_UINT.");
		}

		const u32 INDEX_SIZE = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
		if (indexBuffer.GetSizeInBytes() == 0
			|| indexBuffer.GetSizeInBytes() > std::numeric_limits<u32>::max()
			|| indexBuffer.GetSizeInBytes() % INDEX_SIZE != 0)
		{
			return std::unexpected("Invalid index buffer size.");
		}

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = vertexBuffer.GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer.GetSizeInBytes());
		vertexBufferView.StrideInBytes = vertexStride;

		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		indexBufferView.BufferLocation = indexBuffer.GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = static_cast<UINT>(indexBuffer.GetSizeInBytes());
		indexBufferView.Format = indexFormat;

		return GPUMesh(
			std::move(vertexBuffer),
			vertexBufferView,
			std::move(indexBuffer),
			indexBufferView,
			topology);
	}

	const GPUBuffer& GPUMesh::GetVertexBuffer() const noexcept
	{
		return vertexBuffer;
	}

	u32 GPUMesh::GetVertexCount() const noexcept
	{
		return static_cast<u32>(vertexBufferView.SizeInBytes / vertexBufferView.StrideInBytes);
	}

	D3D12_VERTEX_BUFFER_VIEW GPUMesh::GetD3DVertexBufferView() const noexcept
	{
		return vertexBufferView;
	}

	const GPUBuffer& GPUMesh::GetIndexBuffer() const noexcept
	{
		return indexBuffer;
	}

	u32 GPUMesh::GetIndexCount() const noexcept
	{
		const u32 indexSize = indexBufferView.Format == DXGI_FORMAT_R16_UINT ? 2 : 4;
		return static_cast<u32>(indexBufferView.SizeInBytes / indexSize);
	}

	D3D12_INDEX_BUFFER_VIEW GPUMesh::GetD3DIndexBufferView() const noexcept
	{
		return indexBufferView;
	}

	D3D12_PRIMITIVE_TOPOLOGY GPUMesh::GetD3DTopology() const noexcept
	{
		return topology;
	}
}
