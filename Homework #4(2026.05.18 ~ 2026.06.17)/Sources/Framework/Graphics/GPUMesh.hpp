#pragma once

#include <cstdint>

#include <d3d12.h>

#include "GPUBuffer.hpp"

namespace TUK::Framework
{
	enum class PrimitiveTopology : std::uint32_t
	{
		PointList = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
		LineList = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		LineStrip = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
		TriangleList = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TriangleStrip = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};

	struct GPUMeshCreateInfo final
	{
		std::uint32_t vertexStride = 0;
		DXGI_FORMAT indexFormat = DXGI_FORMAT_R32_UINT;
		PrimitiveTopology topology = PrimitiveTopology::TriangleList;
	};

	// 정점 스트림 하나와 인덱스 버퍼를 소유합니다.
	class GPUMesh
	{
		friend class GPUDevice;

	public:
		~GPUMesh() noexcept = default;

		GPUMesh(const GPUMesh&) = delete;
		GPUMesh& operator=(const GPUMesh&) = delete;

		GPUMesh(GPUMesh&&) noexcept = default;
		GPUMesh& operator=(GPUMesh&&) noexcept = default;

		[[nodiscard]] const GPUBuffer& GetVertexBuffer() const noexcept;
		[[nodiscard]] std::uint32_t GetVertexCount() const noexcept;
		[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetD3DVertexBufferView() const noexcept;

		[[nodiscard]] const GPUBuffer& GetIndexBuffer() const noexcept;
		[[nodiscard]] std::uint32_t GetIndexCount() const noexcept;
		[[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetD3DIndexBufferView() const noexcept;
		
		[[nodiscard]] D3D_PRIMITIVE_TOPOLOGY GetD3DTopology() const noexcept;

	private:
		explicit GPUMesh(
			GPUBuffer vertexBuffer,
			GPUBuffer indexBuffer,
			const GPUMeshCreateInfo& info) noexcept;

		GPUBuffer vertexBuffer;
		std::uint32_t vertexStride;
		GPUBuffer indexBuffer;
		DXGI_FORMAT indexFormat;
		PrimitiveTopology topology;
	};
}
