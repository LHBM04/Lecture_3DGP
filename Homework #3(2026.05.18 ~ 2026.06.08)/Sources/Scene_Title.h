#pragma once

#include "Scene.h"

class Scene_Title final : public Scene
{
public:
	Scene_Title() = default;
	~Scene_Title() override = default;

protected:
	void OnLoad() override;
	void OnUnload() override;
};
