#include "Precompiled.h"
#include "RenderContext.h"

namespace
{
	[[nodiscard]] DirectX::XMFLOAT4X4 IdentityMatrix() noexcept
	{
		DirectX::XMFLOAT4X4 matrix{};
		DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixIdentity());
		return matrix;
	}
}

void RenderContext::Reset()
{
	cameraDescs.clear();
	commands.clear();

	currentShaderId = 0;
	currentMaterialId = 0;
	currentWorld = IdentityMatrix();
	currentVertexBufferViewPtr = nullptr;
	currentIndexBufferViewPtr = nullptr;
}

void RenderContext::AddCamera(const CameraConstants& cameraDesc_)
{
	cameraDescs.push_back(cameraDesc_);
}

void RenderContext::BindShader(uint32_t shaderId_) noexcept
{
	currentShaderId = shaderId_;
}

void RenderContext::BindMaterial(uint32_t materialId_) noexcept
{
	currentMaterialId = materialId_;
}

void RenderContext::SetVertexBuffer(const void* vertexBufferViewPtr_) noexcept
{
	currentVertexBufferViewPtr = vertexBufferViewPtr_;
}

void RenderContext::SetIndexBuffer(const void* indexBufferViewPtr_) noexcept
{
	currentIndexBufferViewPtr = indexBufferViewPtr_;
}

void RenderContext::SetModelMatrix(const DirectX::XMFLOAT4X4& worldMatrix_) noexcept
{
	currentWorld = worldMatrix_;
}

void RenderContext::DrawIndexed(
	RenderPassType passType_,
	uint32_t indexCount_,
	uint32_t startIndexLocation_,
	int32_t baseVertexLocation_)
{
	commands.push_back(DrawCall{
		.sortKey = MakeRenderSortKey(passType_, currentShaderId, currentMaterialId),
		.worldMatrix = currentWorld,
		.vertexBufferViewPtr = currentVertexBufferViewPtr,
		.indexBufferViewPtr = currentIndexBufferViewPtr,
		.indexCount = indexCount_,
		.startIndexLocation = startIndexLocation_,
		.baseVertexLocation = baseVertexLocation_,
	});
}

std::span<const CameraConstants> RenderContext::GetCameraDescriptions() const noexcept
{
	return cameraDescs;
}

std::span<const DrawCall> RenderContext::GetCommands() const noexcept
{
	return commands;
}
