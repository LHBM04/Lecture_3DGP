#pragma once

#include <cstdint>

#include <d3d12.h>

#include "GPUBuffer.hpp"

#include "../Math/ColorRGB.hpp"
#include "../Math/Vector3D.hpp"

namespace TUK::Framework
{
	struct Vertex final
	{
		Vector3D<float> position{ 0.0f, 0.0f, 0.0f };
		Vector3D<float> normal{ 0.0f, 0.0f, 1.0f };
		ColorRGB<float> color{ 1.0f, 1.0f, 1.0f };
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
		
		[[nodiscard]] D3D12_PRIMITIVE_TOPOLOGY GetD3DTopology() const noexcept;

	private:
		explicit GPUMesh(
			GPUBuffer vertexBuffer,
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
			GPUBuffer indexBuffer,
			D3D12_INDEX_BUFFER_VIEW indexBufferView,
			D3D12_PRIMITIVE_TOPOLOGY topology) noexcept;

		GPUBuffer vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

		GPUBuffer indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;

		D3D12_PRIMITIVE_TOPOLOGY topology;
	};
}
