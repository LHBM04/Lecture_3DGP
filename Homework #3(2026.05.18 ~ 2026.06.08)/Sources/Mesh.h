#pragma once

#include <cstdint>
#include <d3d12.h>
#include <vector>
#include <wrl.h>

#include "ColorRGBA.h"
#include "Resource.h"
#include "Vector2D.h"
#include "Vector3D.h"

struct Vertex
{
	Vector3D position;
	Vector3D normal;
};

class Mesh final : public Resource
{
public:
	Mesh() = default;
	~Mesh() override;

	bool Load() override;
	void Unload() override;

	void SetVertices(std::vector<Vertex>&& vertices_);
	void SetIndices(std::vector<uint32_t>&& indices_);

	[[nodiscard]] const std::vector<Vertex>& GetVertices() const noexcept;
	[[nodiscard]] const std::vector<uint32_t>& GetIndices() const noexcept;
	[[nodiscard]] const std::vector<ColorRGBA>& GetColors() const noexcept;
	[[nodiscard]] const std::vector<Vector2D>& GetTextureCoordinates() const noexcept;
	[[nodiscard]] const std::vector<std::vector<uint32_t>>& GetSubMeshes() const noexcept;

	[[nodiscard]] const std::wstring& GetMeshName() const noexcept;
	[[nodiscard]] uint32_t GetSourceVertexCount() const noexcept;

	[[nodiscard]] const Vector3D& GetBoundsCenter() const noexcept;
	[[nodiscard]] const Vector3D& GetBoundsExtents() const noexcept;
	[[nodiscard]] const Vector3D& GetBoundsMin() const noexcept;
	[[nodiscard]] const Vector3D& GetBoundsMax() const noexcept;

	[[nodiscard]] ID3D12Resource* GetVertexBuffer() const noexcept;
	[[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const noexcept;

	[[nodiscard]] ID3D12Resource* GetIndexBuffer() const noexcept;
	[[nodiscard]] const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const noexcept;

	[[nodiscard]] uint32_t GetIndexCount() const noexcept;
	[[nodiscard]] bool HasGpuBuffers() const noexcept;

	bool CreateBuffers(ID3D12Device* device_);

private:
	void ReadVector2(std::ifstream& file_, Vector2D& vector_);
	void ReadVector3(std::ifstream& file_, Vector3D& vector_);
	void ReadColor(std::ifstream& file_, ColorRGBA& color_);
	void UpdateBoundsFromCenterExtents() noexcept;
	void UpdateBoundsFromMinMax() noexcept;
	void RebuildVertices();
	void RebuildFlattenedIndices();

	std::wstring meshName;
	uint32_t sourceVertexCount{ 0 };

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Vector3D> positions;
	std::vector<Vector3D> normals;
	std::vector<Vector2D> textureCoordinates;
	std::vector<ColorRGBA> colors;
	std::vector<std::vector<uint32_t>> subMeshes;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	Vector3D boundsCenter;
	Vector3D boundsExtents;
	Vector3D boundsMin;
	Vector3D boundsMax;
};
