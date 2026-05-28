#pragma once

class RenderContext
{
public:
	void Reset();

	void SetViewport();
	void SetScissorRect();

	void SetShader();

	void BindMaterial();

	void SetVertexBuffer();
	void SetIndexBuffer();

	void DrawArrays();
	void DrawArraysImmediate();

	void DrawElements();
	void DrawElementsImmediate();
};
