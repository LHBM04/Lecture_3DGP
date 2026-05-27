#pragma once

#include "Component.hpp"
#include "ColorRGBA.hpp"

class Mesh;

class MeshRenderer : public Component
{
public:
	virtual ~MeshRenderer() override = default;

	[[nodiscard]] Mesh* GetMesh() const;
	void SetMesh(Mesh* mesh_);

	[[nodiscard]] ColorRGBA GetColor() const;
	void SetColor(const ColorRGBA& color_);

protected:
	virtual void OnRender() override;

private:
	Mesh* mesh{ nullptr };
	ColorRGBA color{ ColorRGBA::GetWhite() };
};

