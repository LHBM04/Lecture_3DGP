#pragma once

#include "ColorRGBA.h"
#include "UIComponent.h"

class Material;

class ImageView final : public UIComponent
{
public:
	[[nodiscard]] const ColorRGBA& GetColor() const noexcept;
	void SetColor(const ColorRGBA& color_) noexcept;

	[[nodiscard]] Material* GetMaterial() noexcept;
	[[nodiscard]] const Material* GetMaterial() const noexcept;
	void SetMaterial(Material* material_) noexcept;

protected:
	virtual void OnRenderUI(RenderTarget& renderTarget_) override;

private:
	ColorRGBA color{ ColorRGBA::GetWhite() };
	Material* material{ nullptr };
};
