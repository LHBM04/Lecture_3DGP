#pragma once

class RenderContext;

class IRenderable
{
public:
	virtual ~IRenderable() = default;

	virtual void OnPreRender(const RenderContext& context_) = 0;
	virtual void OnRender(const RenderContext& context_) = 0;
	virtual void OnPostRender(const RenderContext& context_) = 0;
};