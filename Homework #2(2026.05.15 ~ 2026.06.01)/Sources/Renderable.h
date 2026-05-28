#pragma once

class RenderContext;

class IRenderable
{
public:
	virtual ~IRenderable() = default;

	virtual void OnPreRender(RenderContext& context_) = 0;
	virtual void OnRender(RenderContext& context_) = 0;
	virtual void OnPostRender(RenderContext& context_) = 0;
};
