#pragma once

#include "Scene_Stage.h"

class Scene_Stage1 final : public Scene_Stage
{
public:
	Scene_Stage1() = default;
	~Scene_Stage1() override = default;

protected:
	void OnLoad() override;
	void OnUnload() override;
};
