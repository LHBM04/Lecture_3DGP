#pragma once

#include "Component.h"
#include "Renderable.h"

class MeshRenderer : public Component, 
					 public IRenderable
{
public:
	void OnPreRender(const RenderContext& context_) override;
	void OnRender(const RenderContext& context_) override;
	void OnPostRender(const RenderContext& context_) override;

private:
};
