#pragma once

#include "Asset.h"
#include "Vector2D.h"
#include "Vector3D.h"

struct MeshVertex final
{
	Vector3D position;
	Vector3D normal;
	Vector2D texCoord;
};

class Mesh final : public Asset
{
public:
	Mesh() noexcept = default;
	virtual ~Mesh() noexcept override = default;

	virtual bool Load(const std::filesystem::path& path_) override;
	virtual void Unload() noexcept override;

	[[nodiscard]] const std::vector<MeshVertex>& GetVertices() const noexcept;
	[[nodiscard]] const std::vector<std::uint32_t>& GetIndices() const noexcept;

	void SetData(std::vector<MeshVertex> vertices_, std::vector<std::uint32_t> indices_);

private:
	bool LoadOBJ(const std::filesystem::path& path_);

	std::vector<MeshVertex> vertices;
	std::vector<std::uint32_t> indices;
};
