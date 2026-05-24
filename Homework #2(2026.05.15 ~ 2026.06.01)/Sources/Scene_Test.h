#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Scene.h"
#include "Shader.h"

#include <memory>

struct ID3D12Device;

class Scene_Test final : public Scene
{
public:
	Scene_Test() = default;
	virtual ~Scene_Test() = default;

	Scene_Test(const Scene_Test&) = delete;
	Scene_Test& operator=(const Scene_Test&) = delete;

	Scene_Test(Scene_Test&&) = delete;
	Scene_Test& operator=(Scene_Test&&) = delete;

	[[nodiscard]] bool LoadResources(ID3D12Device* device_);

protected:
	virtual void OnLoad() override;

private:
	std::shared_ptr<Mesh> testMesh;
	std::shared_ptr<Shader> testShader;
	std::shared_ptr<Material> testMaterial;
};
