#include "Precompiled.h"
#include "Scene_Test.h"

#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "ResourceSystem.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"

void Scene_Test::OnLoad()
{
	Mesh* const cubeMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Cube.bin") };
	Material* const cubeMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/Enemy.bin") };
	Material* const defaultMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"DefaultMaterial") };
	if (cubeMesh == nullptr)
	{
		return;
	}

	Shader* const gameObjectShader{ ResourceSystem::GetInstance().GetResource<Shader>(L"Resources/Shaders/GameObject.hlsl") };
	Material* const renderMaterial{ (cubeMaterial != nullptr) ? cubeMaterial : defaultMaterial };
	if (renderMaterial != nullptr)
	{
		if (gameObjectShader != nullptr)
		{
			renderMaterial->SetShader(gameObjectShader);
		}

		renderMaterial->SetCullMode(D3D12_CULL_MODE_NONE);
		renderMaterial->SetDepthEnabled(true);
	}

	GameObject* const cameraObject{ Instantiate(Vector3D(0.0f, 0.0f, -3.0f)) };
	if (cameraObject != nullptr)
	{
		cameraObject->SetName(L"Main Camera");
		if (Camera* const camera{ cameraObject->AddComponent<Camera>() }; camera != nullptr)
		{
			camera->SetAspectRatio(4.0f / 3.0f);
		}
	}

	GameObject* const lightObject{ Instantiate() };
	if (lightObject != nullptr)
	{
		lightObject->SetName(L"Directional Light");
		lightObject->AddComponent<Light>();
	}

	GameObject* const cubeObject{ Instantiate() };
	if (cubeObject == nullptr)
	{
		return;
	}

	cubeObject->SetName(L"Cube");
	if (Transform* const transform{ cubeObject->GetTransform() }; transform != nullptr)
	{
		transform->SetLocalScale(Vector3D(1.0f, 1.0f, 1.0f));
	}

	MeshRenderer* const meshRenderer{ cubeObject->AddComponent<MeshRenderer>() };
	if (meshRenderer == nullptr)
	{
		return;
	}

	meshRenderer->SetMesh(cubeMesh);
	meshRenderer->SetMaterial(renderMaterial);
}
