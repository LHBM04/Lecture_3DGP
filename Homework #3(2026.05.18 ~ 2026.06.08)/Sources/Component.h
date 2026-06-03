#pragma once

#include "Requests.h"

class GameObject;

class Component
{
public:
	virtual ~Component() = default;

	virtual void Update(const TimeContext& time_);
	virtual void OnRender(RenderContext& renderContext_);

	[[nodiscard]] GameObject* GetGameObject() const noexcept;
	void SetGameObject(GameObject* gameObject_) noexcept;

private:
	GameObject* gameObject{ nullptr };
};
