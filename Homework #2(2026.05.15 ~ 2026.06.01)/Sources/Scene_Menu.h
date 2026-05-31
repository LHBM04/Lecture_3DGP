#pragma once

#include "Scene.h"

class Scene_Menu final : public Scene
{
public:
	Scene_Menu() = default;
	~Scene_Menu() override = default;

protected:
	void OnLoad() override;
	void OnUnload() override;
};
