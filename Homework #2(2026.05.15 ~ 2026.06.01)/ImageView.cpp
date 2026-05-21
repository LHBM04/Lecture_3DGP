#include "Precompiled.h"
#include "ImageView.h"

#include "GameObject.h"
#include "Material.h"
#include "RectTransform.h"
#include "RenderTarget.h"

const ColorRGBA& ImageView::GetColor() const noexcept
{
	return color;
}

void ImageView::SetColor(const ColorRGBA& color_) noexcept
{
	color = color_;
}

Material* ImageView::GetMaterial() noexcept
{
	return material;
}

const Material* ImageView::GetMaterial() const noexcept
{
	return material;
}

void ImageView::SetMaterial(Material* material_) noexcept
{
	material = material_;
}

void ImageView::OnRenderUI(RenderTarget& renderTarget_)
{
	const GameObject* const owner{ GetOwner() };
	if (nullptr == owner)
	{
		return;
	}

	const RectTransform* const rectTransform{ owner->GetComponent<RectTransform>() };
	if (nullptr == rectTransform)
	{
		return;
	}

	if (nullptr != material)
	{
		renderTarget_.DrawUIRect(*rectTransform, material->GetAlbedoColor(), *material);
		return;
	}

	renderTarget_.DrawUIRect(*rectTransform, color);
}
