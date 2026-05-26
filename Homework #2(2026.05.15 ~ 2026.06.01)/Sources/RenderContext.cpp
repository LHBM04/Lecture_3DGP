#include "Precompiled.h"
#include "RenderContext.h"

#include "Material.h"
#include "Mesh.h"

void RenderContext::Clear()
{
	drawMeshCommands.clear();
}

void RenderContext::DrawMesh(
	const Mesh& mesh_,
	const Material& material_,
	const Matrix4x4& worldMatrix_)
{
	DrawMesh(mesh_, material_, worldMatrix_, ColorRGBA::GetWhite(), false);
	drawMeshCommands.back().hasColorOverride = false;
}

void RenderContext::DrawMesh(
	const Mesh& mesh_,
	const Material& material_,
	const Matrix4x4& worldMatrix_,
	const ColorRGBA& colorOverride_,
	bool isUI_) noexcept
{
	DrawMeshCommand command{};
	command.mesh = &mesh_;
	command.material = &material_;
	command.worldMatrix = worldMatrix_;
	command.colorOverride = colorOverride_;
	command.hasColorOverride = true;
	command.isUI = isUI_;
	drawMeshCommands.push_back(command);
}

std::span<const DrawMeshCommand> RenderContext::GetDrawMeshCommands() const noexcept
{
	return drawMeshCommands;
}
