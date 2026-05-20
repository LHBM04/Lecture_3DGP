#pragma once

#include "Scene.h"

class Mesh;

class Scene_Title final : public Scene
{
protected:
	virtual void OnLoad() override;
	virtual void OnUpdate() override;
	virtual void OnUnload() override;

private:
	Mesh* cube{ nullptr };
};
