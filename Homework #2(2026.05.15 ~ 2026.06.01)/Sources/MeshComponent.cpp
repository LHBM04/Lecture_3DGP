#include "Precompiled.h"
#include "MeshComponent.h"

#include "RenderSystem.h"
#include "Mesh.h"
#include "Material.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

void MeshComponent::OnRender()
{
	if (mesh == nullptr)
	{
		return;
	}

	RenderSystem::GetInstance().QueueMesh(mesh, material, GetOwner()->GetComponent<Transform>()->GetWorldMatrix());
}
