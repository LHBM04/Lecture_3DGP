#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include <DirectXMath.h>

#include "CameraConstants.h"
#include "RenderCommand.h"

class RenderContext final
{
public:
	RenderContext() = default;
	~RenderContext() = default;

	RenderContext(const RenderContext&) = delete;
	RenderContext& operator=(const RenderContext&) = delete;

	RenderContext(RenderContext&&) = delete;
	RenderContext& operator=(RenderContext&&) = delete;

	void Reset();

	void AddCamera(const CameraConstants& cameraDesc_);

	void BindShader(uint32_t shaderId_) noexcept;
	void BindMaterial(uint32_t materialId_) noexcept;
	void SetVertexBuffer(const void* vertexBufferViewPtr_) noexcept;
	void SetIndexBuffer(const void* indexBufferViewPtr_) noexcept;
	void SetModelMatrix(const DirectX::XMFLOAT4X4& worldMatrix_) noexcept;

	void DrawIndexed(
		RenderPassType passType_,
		uint32_t indexCount_,
		uint32_t startIndexLocation_ = 0,
		int32_t baseVertexLocation_ = 0);

	[[nodiscard]] std::span<const CameraConstants> GetCameraDescriptions() const noexcept;
	[[nodiscard]] std::span<const DrawCall> GetCommands() const noexcept;

private:
	std::vector<CameraConstants> cameraDescs;
	std::vector<DrawCall> commands;

	uint32_t currentShaderId{ 0 };
	uint32_t currentMaterialId{ 0 };
	DirectX::XMFLOAT4X4 currentWorld{};
	const void* currentVertexBufferViewPtr{ nullptr };
	const void* currentIndexBufferViewPtr{ nullptr };
};
