#include "Precompiled.h"
#include "Scene_Title.h"

#include "AssetManager.h"
#include "Mesh.h"
#include "Renderer.h"

void Scene_Title::OnLoad()
{
	cube = AssetManager::Load<Mesh>(L"Assets/Cube.obj");
}

void Scene_Title::OnRender(Renderer& renderer_)
{
	if (nullptr != cube)
	{
		renderer_.DrawMesh(*cube);
	}
}

void Scene_Title::OnUnload()
{
	AssetManager::Unload(cube);
	cube = nullptr;
}
