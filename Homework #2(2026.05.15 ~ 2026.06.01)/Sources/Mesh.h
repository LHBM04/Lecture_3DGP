#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <vector>
#include <cstdint>

#include "Resource.h"
#include "Vector3D.h"
#include "ColorRGBA.h"

struct Vertex
{
	Vector3D position;
	ColorRGBA color;
};

class Mesh final : public Resource
{
public:
	Mesh() = default;
	~Mesh() override = default;

	void SetVertices(std::vector<Vertex>&& vertices_);
	void SetIndices(std::vector<uint32_t>&& indices_);

	[[nodiscard]] const std::vector<Vertex>& GetVertices() const;
	[[nodiscard]] const std::vector<uint32_t>& GetIndices() const;

	[[nodiscard]] ID3D12Resource* GetVertexBuffer() const;
	[[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const;

	[[nodiscard]] ID3D12Resource* GetIndexBuffer() const;
	[[nodiscard]] const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const;

	[[nodiscard]] uint32_t GetIndexCount() const;

	bool CreateBuffers(ID3D12Device* device_);

private:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
};

inline void Mesh::SetVertices(std::vector<Vertex>&& vertices_)
{
	vertices = std::move(vertices_);
}

inline void Mesh::SetIndices(std::vector<uint32_t>&& indices_)
{
	indices = std::move(indices_);
}

inline const std::vector<Vertex>& Mesh::GetVertices() const
{
	return vertices;
}

inline const std::vector<uint32_t>& Mesh::GetIndices() const
{
	return indices;
}

inline ID3D12Resource* Mesh::GetVertexBuffer() const
{
	return vertexBuffer.Get();
}

inline const D3D12_VERTEX_BUFFER_VIEW& Mesh::GetVertexBufferView() const
{
	return vertexBufferView;
}

inline ID3D12Resource* Mesh::GetIndexBuffer() const
{
	return indexBuffer.Get();
}

inline const D3D12_INDEX_BUFFER_VIEW& Mesh::GetIndexBufferView() const
{
	return indexBufferView;
}

inline uint32_t Mesh::GetIndexCount() const
{
	return static_cast<uint32_t>(indices.size());
}
