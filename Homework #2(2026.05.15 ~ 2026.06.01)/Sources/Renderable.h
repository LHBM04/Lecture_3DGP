#pragma once

class Renderable
{
public:
	virtual ~Renderable() = default;
	virtual void OnRender() = 0;
};
