#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Scene.h"
#include "Shader.h"

#include <memory>
#include <vector>

struct ID3D12Device;

class Scene_Test final : public Scene
{
public:
	~Scene_Test() override = default;

	[[nodiscard]] bool LoadResources(ID3D12Device* device_);

protected:
	virtual void OnLoad() override;

private:
	std::shared_ptr<Mesh> testMesh;
	std::shared_ptr<Mesh> stairsMesh;
	std::shared_ptr<Mesh> playerMesh;
	std::shared_ptr<Mesh> uiQuadMesh;
	std::shared_ptr<Shader> testShader;
	std::shared_ptr<Shader> uiShader;
	std::shared_ptr<Material> cubeMaterial;
	std::shared_ptr<Material> stairsMaterial;
	std::shared_ptr<Material> playerMaterial;
	std::shared_ptr<Material> uiMaterial;
	std::shared_ptr<class Font> uiFont;
};
