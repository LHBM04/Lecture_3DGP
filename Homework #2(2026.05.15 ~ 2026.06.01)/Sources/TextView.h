#pragma once

#include "ColorRGBA.h"
#include "Component.h"
#include "RenderableUI.h"
#include "Vector2D.h"

class RectTransform;
class Font;
class Material;
class Mesh;

class TextView final : public Component, public RenderableUI
{
public:
	[[nodiscard]] const std::wstring& GetText() const noexcept;
	void SetText(const std::wstring& text_) noexcept;

	[[nodiscard]] Font* GetFont() const noexcept;
	void SetFont(Font* font_) noexcept;

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
	void MarkDirty() noexcept;
	void RebuildMeshIfDirty(RectTransform& rectTransform_);

	std::wstring text{ L"TextView" };
	Font* font{ nullptr };
	Mesh* mesh{ nullptr };
	std::unique_ptr<Mesh> cachedMesh;
	Material* material{ nullptr };
	ColorRGBA color{ ColorRGBA::GetWhite() };
	bool hasLoggedMissingFont{ false };
	bool isDirty{ true };
	Vector2D cachedAnchoredPosition{ 0.0f, 0.0f };
	Vector2D cachedSize{ 0.0f, 0.0f };
	Vector2D cachedPivot{ 0.0f, 0.0f };
	int cachedScreenWidth{ 0 };
	int cachedScreenHeight{ 0 };
	int cachedFontSize{ 0 };
};
