#pragma once

class RenderContext;

class Renderable
{
public:
	virtual ~Renderable() = default;
	virtual void OnRender(RenderContext& context_) = 0;
};
