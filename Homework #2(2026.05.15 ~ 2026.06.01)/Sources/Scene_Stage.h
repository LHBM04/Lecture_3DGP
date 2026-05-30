#pragma once

#include "Scene.h"
#include <string>
#include <string_view>
#include <fstream>

class Scene_Stage : public Scene
{
public:
	Scene_Stage() = default;
	~Scene_Stage() override = default;

	void BuildSceneObjects(std::wstring_view mapPath_);

private:
	bool ReadTag(std::ifstream& file_, const std::string& expectedTag_);
	std::wstring ReadString(std::ifstream& file_);
};
