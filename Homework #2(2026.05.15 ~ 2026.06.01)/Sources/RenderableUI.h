#pragma once

class RenderableUI
{
public:
	virtual ~RenderableUI() = default;
	virtual void OnRenderUI() = 0;
};
