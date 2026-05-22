#include "Precompiled.h"
#include "UIComponent.h"

#include "GameObject.h"
#include "Scene.h"

void UIComponent::RenderUI(RenderTarget& renderTarget_)
{
	OnRenderUI(renderTarget_);
}

void UIComponent::OnAttach()
{
	GameObject* const owner{ GetOwner() };
	if (nullptr != owner && nullptr != owner->GetCurrentScene())
	{
		owner->GetCurrentScene()->AddUIComponent(this);
	}
}

void UIComponent::OnDetach()
{
	GameObject* const owner{ GetOwner() };
	if (nullptr != owner && nullptr != owner->GetCurrentScene())
	{
		owner->GetCurrentScene()->RemoveUIComponent(this);
	}
}
