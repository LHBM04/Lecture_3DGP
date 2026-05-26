#pragma once

#include "ColorRGBA.h"
#include "Component.h"
#include "RenderableUI.h"

class Material;
class Mesh;
class MeshRenderer;

class ImageView final : public Component, public RenderableUI
{
public:
	ImageView() = default;
	~ImageView() override = default;

	[[nodiscard]] Mesh* GetMesh() const noexcept;
	void SetMesh(Mesh* mesh_) noexcept;

	[[nodiscard]] Material* GetMaterial() const noexcept;
	void SetMaterial(Material* material_) noexcept;

	[[nodiscard]] const ColorRGBA& GetColor() const noexcept;
	void SetColor(const ColorRGBA& color_) noexcept;

protected:
	void OnAttach() override;
	void OnRenderUI(RenderContext& context_) override;

private:
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
	ColorRGBA color{ ColorRGBA::GetWhite() };
};
