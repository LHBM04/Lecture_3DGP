#pragma once

#include "ColorRGBA.h"
#include "Matrix4x4.h"

#include <span>
#include <vector>

class Material;
class Mesh;

struct DrawMeshCommand final
{
	const Mesh* mesh{ nullptr };
	const Material* material{ nullptr };
	Matrix4x4 worldMatrix{};
	ColorRGBA colorOverride{};
	bool hasColorOverride{ false };
	bool isUI{ false };
};

class RenderContext final
{
public:
	void Clear();

	void DrawMesh(
		const Mesh& mesh_,
		const Material& material_,
		const Matrix4x4& worldMatrix_);

	void DrawMesh(
		const Mesh& mesh_,
		const Material& material_,
		const Matrix4x4& worldMatrix_,
		const ColorRGBA& colorOverride_,
		bool isUI_) noexcept;

	[[nodiscard]] std::span<const DrawMeshCommand> GetDrawMeshCommands() const noexcept;

private:
	std::vector<DrawMeshCommand> drawMeshCommands;
};
