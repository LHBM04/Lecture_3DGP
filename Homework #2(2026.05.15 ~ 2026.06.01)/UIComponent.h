#pragma once

#include "Component.h"

class Renderer;

class UIComponent : public Component
{
public:
	void RenderUI(Renderer& renderer_);

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnRenderUI(Renderer&) {}
};
