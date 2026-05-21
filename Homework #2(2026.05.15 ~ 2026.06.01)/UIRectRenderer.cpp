#include "Precompiled.h"
#include "UIRectRenderer.h"

#include "GameObject.h"
#include "RectTransform.h"
#include "Renderer.h"

const ColorRGBA& UIRectRenderer::GetColor() const noexcept
{
	return color;
}

void UIRectRenderer::SetColor(const ColorRGBA& color_) noexcept
{
	color = color_;
}

void UIRectRenderer::OnRenderUI(Renderer& renderer_)
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

	renderer_.DrawUIRect(*rectTransform, color);
}
