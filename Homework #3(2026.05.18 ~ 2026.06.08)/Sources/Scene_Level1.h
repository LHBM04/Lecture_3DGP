#pragma once

#include "Scene.h"

class Scene_Level1 : public Scene
{
public:
	Scene_Level1() noexcept = default;
	~Scene_Level1() noexcept = default;

protected:
	void OnLoad() override;
	void OnUnload() override;
};
