#include "Precompiled.h"
#include "ImageView.h"

#include "GameObject.h"
#include "Material.h"
#include "RectTransform.h"
#include "Renderer.h"

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

void ImageView::OnRenderUI(Renderer& renderer_)
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
		renderer_.DrawUIRect(*rectTransform, material->GetAlbedoColor(), *material);
		return;
	}

	renderer_.DrawUIRect(*rectTransform, color);
}
