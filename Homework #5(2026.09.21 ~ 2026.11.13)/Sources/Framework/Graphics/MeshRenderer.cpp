#include "Precompiled.hpp"
#include "MeshRenderer.hpp"

#include <iostream>

#include "../Resources/MaterialAsset.hpp"
#include "../Resources/MeshAsset.hpp"

#include "GPUCommandBuffer.hpp"

namespace TUK::Framework
{
	void MeshRenderer::OnRender(GPUCommandBuffer& commandBuffer)
	{
		if (!mesh || !material || !mesh->HasGPUMesh() || !material->HasGraphicsPipelineState())
		{
			return;
		}

		if (auto result = commandBuffer.SetMaterial(*material); !result)
		{
			std::cerr << "MeshRenderer 머티리얼 적용 실패: " << result.error() << std::endl;
			return;
		}

		if (auto result = commandBuffer.SetEntity(GetOwner()); !result)
		{
			std::cerr << "MeshRenderer 오브젝트 변환 적용 실패: " << result.error() << std::endl;
			return;
		}

		const GPUMesh& gpuMesh = mesh->GetGPUMesh();
		commandBuffer.SetGPUMesh(gpuMesh);
		commandBuffer.DrawIndexed(gpuMesh.GetIndexCount());
	}
}
