#include "Precompiled.hpp"
#include "MeshRenderer.hpp"

#include <iostream>

#include "../Resources/MaterialAsset.hpp"
#include "../Resources/MeshAsset.hpp"

#include "GPUCommandBuffer.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	void MeshRenderer::OnRender(GPUCommandBuffer& commandBuffer)
	{
		if (!mesh || !material || !mesh->HasGPUMesh())
		{
			return;
		}

		const GPUGraphicsPipelineState* pipeline = material->GetGraphicsPipelineState();
		if (!pipeline)
		{
			return;
		}

		if (auto result = commandBuffer.SetMaterial(*material, *pipeline, material->GetTargetFormat()); !result)
		{
			Logger::LogError("{}{}", "MeshRenderer failed to apply material: ", result.error());
			return;
		}

		if (auto result = commandBuffer.SetEntity(GetOwner()); !result)
		{
			Logger::LogError("{}{}", "MeshRenderer failed to apply object transform: ", result.error());
			return;
		}

		const GPUMesh& gpuMesh = mesh->GetGPUMesh();
		commandBuffer.SetGPUMesh(gpuMesh);
		commandBuffer.DrawIndexed(gpuMesh.GetIndexCount());
	}
}
