#pragma once

#include "Component.h"

class Mesh;
class Renderer;

class MeshRenderer final : public Component
{
public:
	MeshRenderer() noexcept = default;
	virtual ~MeshRenderer() noexcept override = default;

	void SetMesh(Mesh* mesh_) noexcept;
	[[nodiscard]] Mesh* GetMesh() noexcept;
	[[nodiscard]] const Mesh* GetMesh() const noexcept;

protected:
	virtual void OnRender(Renderer& renderer_) override;

private:
	Mesh* mesh{ nullptr };
};
