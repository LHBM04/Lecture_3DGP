#pragma once

#include "Scene.h"

class Mesh;
class Renderer;

class Scene_Title final : public Scene
{
protected:
	virtual void OnLoad() override;
	virtual void OnRender(Renderer& renderer_) override;
	virtual void OnUnload() override;

private:
	Mesh* cube{ nullptr };
};
