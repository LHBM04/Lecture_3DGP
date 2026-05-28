#pragma once

#include "Component.h"
#include "Renderable.h"

class MeshRenderer : public Component, 
					 public IRenderable
{
public:
	void OnPreRender(RenderContext& context_) override;
	void OnRender(RenderContext& context_) override;
	void OnPostRender(RenderContext& context_) override;

private:
};
