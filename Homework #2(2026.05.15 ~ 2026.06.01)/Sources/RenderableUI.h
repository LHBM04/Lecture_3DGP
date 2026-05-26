#pragma once

class RenderContext;

class RenderableUI
{
public:
	virtual ~RenderableUI() = default;
	virtual void OnRenderUI(RenderContext& context_) = 0;
};
