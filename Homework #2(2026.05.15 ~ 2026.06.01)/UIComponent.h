#pragma once

#include "Component.h"

class RenderTarget;

class UIComponent : public Component
{
public:
	void RenderUI(RenderTarget& renderTarget_);

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnRenderUI(RenderTarget&) {}
};
