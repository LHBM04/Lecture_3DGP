#pragma once

#include "Scene.h"

class RenderTarget;

namespace SceneManager
{
	void Initialize();
	void Update();
	void Render(RenderTarget& renderTarget_);

	void LoadScene(std::size_t index_);
	void LoadScene(const std::string& name_);

	void UnloadScene();
};
