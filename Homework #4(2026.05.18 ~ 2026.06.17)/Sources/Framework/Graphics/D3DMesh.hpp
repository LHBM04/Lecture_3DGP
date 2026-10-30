#pragma once

#include <cstdint>

#include <d3d12.h>

#include "D3DBuffer.hpp"

namespace TUK::Framework
{
	// 정점 스트림 하나와 인덱스 버퍼를 소유합니다.
	class D3DMesh
	{
		friend class D3DDevice;

	public:
		~D3DMesh() noexcept = default;

		D3DMesh(const D3DMesh&) = delete;
		D3DMesh& operator=(const D3DMesh&) = delete;

		D3DMesh(D3DMesh&&) noexcept = default;
		D3DMesh& operator=(D3DMesh&&) noexcept = default;

		[[nodiscard]] const D3DBuffer& GetVertexBuffer() const noexcept;
		[[nodiscard]] std::uint32_t GetVertexCount() const noexcept;
		[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetD3DVertexBufferView() const noexcept;

		[[nodiscard]] const D3DBuffer& GetIndexBuffer() const noexcept;
		[[nodiscard]] std::uint32_t GetIndexCount() const noexcept;
		[[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetD3DIndexBufferView() const noexcept;
		
		[[nodiscard]] D3D12_PRIMITIVE_TOPOLOGY GetD3DTopology() const noexcept;

	private:
		explicit D3DMesh(
			D3DBuffer vertexBuffer,
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
			D3DBuffer indexBuffer,
			D3D12_INDEX_BUFFER_VIEW indexBufferView,
			D3D12_PRIMITIVE_TOPOLOGY topology) noexcept;

		D3DBuffer vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

		D3DBuffer indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;

		D3D12_PRIMITIVE_TOPOLOGY topology;
	};
}
