#include "Precompiled.h"
#include "Component.h"

void Component::Update(const TimeContext& time_)
{
	(void)time_;
}

void Component::OnRender(RenderContext& renderContext_)
{
	(void)renderContext_;
}

GameObject* Component::GetGameObject() const noexcept
{
	return gameObject;
}

void Component::SetGameObject(GameObject* gameObject_) noexcept
{
	gameObject = gameObject_;
}
