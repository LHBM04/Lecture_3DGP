#pragma once

#include "Resource.h"

#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <filesystem>
#include <span>

class Mesh final : public Resource
{
public:
	static void SetDefaultDevice(ID3D12Device* device_) noexcept;
	[[nodiscard]] static ID3D12Device* GetDefaultDevice() noexcept;

	struct Vertex final
	{
		float position[3];
		float normal[3];
		float texCoord[2];
	};

	virtual ~Mesh() = default;

	bool Load(const std::filesystem::path& path_) override;
	void Unload() override;

	[[nodiscard]] uint64_t GetId() const noexcept;
	void SetId(uint64_t id_) noexcept;

	[[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const noexcept;
	void SetVertexBufferView(const D3D12_VERTEX_BUFFER_VIEW& view_) noexcept;

	[[nodiscard]] bool HasIndexBuffer() const noexcept;

	[[nodiscard]] const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const noexcept;
	void SetIndexBufferView(const D3D12_INDEX_BUFFER_VIEW& view_) noexcept;
	void ClearIndexBufferView() noexcept;

	[[nodiscard]] UINT GetVertexCount() const noexcept;
	void SetVertexCount(UINT count_) noexcept;

	[[nodiscard]] UINT GetIndexCount() const noexcept;
	void SetIndexCount(UINT count_) noexcept;

	[[nodiscard]] bool LoadFromBinary(ID3D12Device* device_, const std::filesystem::path& path_);
	[[nodiscard]] bool BuildFromRaw(
		ID3D12Device* device_,
		std::span<const Vertex> vertices_,
		std::span<const std::uint32_t> indices_);
	[[nodiscard]] bool BuildFromRaw(
		std::span<const Vertex> vertices_,
		std::span<const std::uint32_t> indices_);

private:
	static ID3D12Device* defaultDevice;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	uint64_t id{ 0 };
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	bool hasIndexBuffer{ false };
	UINT vertexCount{ 0 };
	UINT indexCount{ 0 };
};
