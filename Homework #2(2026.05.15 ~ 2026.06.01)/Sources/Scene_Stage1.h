#pragma once

#include <fstream>
#include <string>
#include <string_view>

#include "Scene.h"

class Scene_Stage1 final : public Scene
{
public:
	Scene_Stage1() = default;
	~Scene_Stage1() override = default;

protected:
	void OnLoad() override;
	void OnUnload() override;

private:
	void BuildSceneObjects(std::wstring_view mapPath_);
	bool ReadTag(std::ifstream& file_, const std::string& expectedTag_);
	std::wstring ReadString(std::ifstream& file_);
};
