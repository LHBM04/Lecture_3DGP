#include "Precompiled.hpp"
#include "MeshRenderer.hpp"

#include "../Resources/MaterialAsset.hpp"
#include "../Resources/MeshAsset.hpp"

#include "../Utilities/Logger.hpp"

#include "GPUCommandBuffer.hpp"

namespace TUK::Framework
{
	void MeshRenderer::OnRender(GPUCommandBuffer& commandBuffer)
	{
		if (!mesh || !material)
		{
			return;
		}

		const GPUMesh* gpuMesh = mesh->GetGPUMesh();
		const GPUGraphicsPipelineState* pipeline = material->GetGraphicsPipelineState();
		if (!gpuMesh || !pipeline)
		{
			return;
		}

		if (auto result = commandBuffer.SetMaterial(material, *pipeline, material->GetTargetFormat()); !result)
		{
			Logger::LogError("{}{}", "MeshRenderer failed to apply material: ", result.error());
			return;
		}

		if (auto result = commandBuffer.SetEntity(GetOwner()); !result)
		{
			Logger::LogError("{}{}", "MeshRenderer failed to apply object transform: ", result.error());
			return;
		}

		commandBuffer.SetGPUMesh(*gpuMesh);
		commandBuffer.DrawIndexed(gpuMesh->GetIndexCount());
	}
}
