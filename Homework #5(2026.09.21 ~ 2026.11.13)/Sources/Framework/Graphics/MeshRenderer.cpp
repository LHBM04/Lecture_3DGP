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
				std::cerr << "MeshRenderer 癒명꽣由ъ뼹 ?곸슜 ?ㅽ뙣: " << result.error() << std::endl;
				return;
			}
			
			if (auto result = commandBuffer.SetObjectTransform(part.localToModel * worldMatrix); !result)
			{
				std::cerr << "MeshRenderer ?ㅻ툕?앺듃 蹂???곸슜 ?ㅽ뙣: " << result.error() << std::endl;
				return;
			}
			
			commandBuffer.SetGPUMesh(part.mesh);
			commandBuffer.DrawIndexed(part.mesh.GetIndexCount());
		}
	}
}
