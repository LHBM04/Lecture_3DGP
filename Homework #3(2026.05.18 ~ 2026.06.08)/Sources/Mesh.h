#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include <d3d12.h>
#include <wrl.h>

#include "Resource.h"
#include "Vector3D.h"

class Mesh final : public Resource
{
public:
	struct Vertex final
	{
		Vector3D position;
		Vector3D normal;
	};

	Mesh(std::wstring_view resourceName_ = {}, std::filesystem::path sourcePath_ = {});
	~Mesh() override = default;

	bool Load() override;
	void Unload() override;

	bool Create(
		std::span<const std::byte> vertexData_,
		UINT vertexStride_,
		UINT vertexCount_,
		std::span<const uint32_t> indices_ = {});

	template <class TVertex>
	bool Create(std::span<const TVertex> vertices_, std::span<const uint32_t> indices_ = {})
	{
		return Create(
			std::as_bytes(vertices_),
			static_cast<UINT>(sizeof(TVertex)),
			static_cast<UINT>(vertices_.size()),
			indices_);
	}

	void Bind(ID3D12GraphicsCommandList* commandList_) const;
	void Draw(ID3D12GraphicsCommandList* commandList_) const;

	[[nodiscard]] const Vector3D& GetBoundsMin() const noexcept;
	[[nodiscard]] const Vector3D& GetBoundsMax() const noexcept;
	[[nodiscard]] UINT GetVertexCount() const noexcept;
	[[nodiscard]] UINT GetIndexCount() const noexcept;
	[[nodiscard]] UINT GetVertexStride() const noexcept;
	[[nodiscard]] bool HasGpuBuffers() const noexcept;
	bool CreateBuffers(ID3D12Device* device_);

	void Release();

private:
	static bool ReadTag(std::ifstream& file_, const std::string& expectedTag_);
	static void ReadVector3(std::ifstream& file_, Vector3D& vector_);

	std::vector<std::byte> cpuVertexData;
	std::vector<uint32_t> cpuIndices;
	Vector3D boundsMin{ Vector3D::GetZero() };
	Vector3D boundsMax{ Vector3D::GetZero() };

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	UINT vertexCount{ 0 };
	UINT indexCount{ 0 };
	UINT vertexStride{ 0 };
};
