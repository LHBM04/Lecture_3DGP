#include "Precompiled.hpp"
#include "MeshRenderer.hpp"

#include "../Resources/MaterialAsset.hpp"
#include "../Resources/MeshAsset.hpp"

#include "../Core/Common.hpp"

#include "GPUCommandBuffer.hpp"

namespace TUK::Framework
{
	void MeshRenderer::OnRender(GPUCommandBuffer& commandBuffer)
	{
		if (!mesh || !material)
		{
			return;
		}

		const GPUMesh& gpuMesh = mesh->GetGPUMesh();
		const GPUGraphicsPipelineState& pipeline = material->GetGraphicsPipelineState();

		if (auto result = commandBuffer.SetMaterial(material, pipeline, material->GetTargetFormat()); !result)
		{
			LOG_ERROR("메시 렌더러의 머티리얼 적용에 실패했습니다: {}", result.error());
			return;
		}

		if (auto result = commandBuffer.SetEntity(GetOwner()); !result)
		{
			LOG_ERROR("메시 렌더러의 오브젝트 변환 적용에 실패했습니다: {}", result.error());
			return;
		}

		commandBuffer.SetGPUMesh(gpuMesh);
		commandBuffer.DrawIndexed(gpuMesh.GetIndexCount());
	}
}
