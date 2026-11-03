#include "Precompiled.hpp"
#include "MeshRenderer.hpp"

#include <iostream>

#include "../Resources/ModelAsset.hpp"

#include "../Entities/Entity.hpp"
#include "../Entities/Transform.hpp"

#include "GPUCommandBuffer.hpp"

namespace TUK::Framework
{
	void MeshRenderer::OnRender(GPUCommandBuffer& commandBuffer)
	{
		if (!model)
		{
			return;
		}
		const Transform* transform = GetOwner().GetComponent<Transform>();
		const Matrix4x4<float> worldMatrix = transform ? transform->GetWorldMatrix() : Matrix4x4<float>::GetIdentity();
		for (const ModelPart& part : model->GetParts())
		{
			const MaterialAsset* partMaterial = material ? material : part.material;
			if (!partMaterial)
			{
				continue;
			}
			if (auto result = commandBuffer.SetMaterial(*partMaterial); !result)
			{
				std::cerr << "MeshRenderer 머터리얼 적용 실패: " << result.error() << std::endl;
				return;
			}
			if (auto result = commandBuffer.SetObjectTransform(part.localToModel * worldMatrix); !result)
			{
				std::cerr << "MeshRenderer 오브젝트 변환 적용 실패: " << result.error() << std::endl;
				return;
			}
			commandBuffer.SetGPUMesh(part.mesh);
			commandBuffer.DrawIndexed(part.mesh.GetIndexCount());
		}
	}
}
