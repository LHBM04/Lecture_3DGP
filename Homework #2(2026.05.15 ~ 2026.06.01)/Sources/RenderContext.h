#pragma once

#include "ColorRGBA.h"
#include "Matrix4x4.h"

#include <d3d12.h>
#include <cstdint>

class Camera;
class Light;
class Material;
class Renderer;
class Shader;

class RenderContext final
{
public:
	void Clear();
	void ResetViewport();

	void SetCamera(const Camera* camera_);
	void SetLight(const Light* light_);

	void UseProgram(const Shader* shader_);
	void BindVertexBuffer(
		const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView_,
		UINT vertexCount_,
		uint64_t meshId_,
		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		UINT firstVertex_ = 0);
	void BindElementBuffer(
		const D3D12_INDEX_BUFFER_VIEW& indexBufferView_,
		UINT indexCount_,
		UINT firstIndex_ = 0,
		INT baseVertexLocation_ = 0);
	void BindMaterial(const Material* material_, const ColorRGBA* overrideColor_ = nullptr);
	void SetModelMatrix(const Matrix4x4& modelMatrix_);

	void DrawArrays();
	void DrawElements();
	void DrawUIArrays();
	void DrawUIElements();

private:
	friend class Renderer;
	explicit RenderContext(Renderer& renderer_) noexcept;

private:
	Renderer* renderer{ nullptr };
};
