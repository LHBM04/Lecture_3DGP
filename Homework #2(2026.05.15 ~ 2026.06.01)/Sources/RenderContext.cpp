#include "Precompiled.h"
#include "RenderContext.h"

#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Renderer.h"
#include "Shader.h"
#include "Vector4D.h"

RenderContext::RenderContext(Renderer& renderer_) noexcept
	: renderer{ &renderer_ }
{
}

void RenderContext::Clear()
{
	if (nullptr == renderer)
	{
		return;
	}

	if (CameraClearMode::None == renderer->currentCameraClearMode)
	{
		return;
	}

	Renderer::BackBuffer& backBuffer{ renderer->backBuffers[renderer->currentBackBufferIndex] };
	D3D12_CPU_DESCRIPTOR_HANDLE rtv{ backBuffer.rtv };
	D3D12_CPU_DESCRIPTOR_HANDLE dsv{ renderer->dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	if (CameraClearMode::SolidColor == renderer->currentCameraClearMode)
	{
		const FLOAT color[4]
		{
			renderer->currentCameraClearColor.x,
			renderer->currentCameraClearColor.y,
			renderer->currentCameraClearColor.z,
			renderer->currentCameraClearColor.w
		};
		renderer->commandList->ClearRenderTargetView(rtv, color, 0, nullptr);
	}

	renderer->commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void RenderContext::ResetViewport()
{
	if (nullptr == renderer)
	{
		return;
	}

	D3D12_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(renderer->options.width);
	viewport.Height = static_cast<float>(renderer->options.height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	renderer->commandList->RSSetViewports(1, &viewport);

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = renderer->options.width;
	scissorRect.bottom = renderer->options.height;
	renderer->commandList->RSSetScissorRects(1, &scissorRect);
}

void RenderContext::SetCamera(const Camera* camera_)
{
	if (nullptr == renderer || nullptr == camera_)
	{
		return;
	}

	renderer->currentCameraConstants = {};

	const Vector4D& viewportRect{ camera_->GetViewport() };
	const float targetWidth{ static_cast<float>(std::max(1, renderer->options.width)) };
	const float targetHeight{ static_cast<float>(std::max(1, renderer->options.height)) };

	D3D12_VIEWPORT viewport{};
	viewport.TopLeftX = viewportRect.x * targetWidth;
	viewport.TopLeftY = viewportRect.y * targetHeight;
	viewport.Width = std::max(1.0f, viewportRect.z * targetWidth);
	viewport.Height = std::max(1.0f, viewportRect.w * targetHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	renderer->commandList->RSSetViewports(1, &viewport);

	D3D12_RECT scissorRect{};
	scissorRect.left = static_cast<LONG>(viewport.TopLeftX);
	scissorRect.top = static_cast<LONG>(viewport.TopLeftY);
	scissorRect.right = static_cast<LONG>(viewport.TopLeftX + viewport.Width);
	scissorRect.bottom = static_cast<LONG>(viewport.TopLeftY + viewport.Height);
	renderer->commandList->RSSetScissorRects(1, &scissorRect);

	const float aspect{ viewport.Width / viewport.Height };

	renderer->currentCameraConstants.view = camera_->GetViewMatrix();
	renderer->currentCameraConstants.projection = camera_->GetProjectionMatrix(aspect);
	renderer->currentCameraConstants.viewProjection = camera_->GetViewProjectionMatrix(aspect);
	renderer->currentCameraClearMode = camera_->GetClearMode();
	renderer->currentCameraClearColor = camera_->GetClearColor();
}

void RenderContext::SetLight(const Light* light_)
{
	if (nullptr == renderer || nullptr == light_)
	{
		return;
	}

	renderer->currentLightDirection = light_->GetDirection();
	renderer->currentLightColor = light_->GetColor();
	renderer->currentLightIntensity = light_->GetIntensity();
}

void RenderContext::UseProgram(const Shader* shader_)
{
	if (nullptr == renderer || nullptr == shader_)
	{
		return;
	}

	renderer->drawState.pipelineState = const_cast<ID3D12PipelineState*>(shader_->GetPipelineState());
	renderer->drawState.graphicsRootSignature = const_cast<ID3D12RootSignature*>(shader_->GetGraphicsRootSignature());
	renderer->drawState.pipelineId = shader_->GetPipelineId();

	renderer->drawState.cameraSlot = UINT_MAX;
	renderer->drawState.objectSlot = UINT_MAX;
	renderer->drawState.materialSlot = UINT_MAX;
	renderer->drawState.lightSlot = UINT_MAX;
	(void)shader_->TryGetRootParameterIndex("Camera", renderer->drawState.cameraSlot);
	(void)shader_->TryGetRootParameterIndex("Object", renderer->drawState.objectSlot);
	(void)shader_->TryGetRootParameterIndex("Material", renderer->drawState.materialSlot);
	(void)shader_->TryGetRootParameterIndex("Light", renderer->drawState.lightSlot);
}

void RenderContext::BindVertexBuffer(
	const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView_,
	UINT vertexCount_,
	uint64_t meshId_,
	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_,
	UINT firstVertex_)
{
	if (nullptr == renderer)
	{
		return;
	}

	renderer->drawState.primitiveTopology = primitiveTopology_;
	renderer->drawState.vertexBufferView = vertexBufferView_;
	renderer->drawState.vertexCount = vertexCount_;
	renderer->drawState.startVertexLocation = firstVertex_;
	renderer->drawState.instanceCount = 1;
	renderer->drawState.startInstanceLocation = 0;
	renderer->drawState.meshId = meshId_;
}

void RenderContext::BindElementBuffer(
	const D3D12_INDEX_BUFFER_VIEW& indexBufferView_,
	UINT indexCount_,
	UINT firstIndex_,
	INT baseVertexLocation_)
{
	if (nullptr == renderer)
	{
		return;
	}

	renderer->drawState.indexBufferView = indexBufferView_;
	renderer->drawState.indexCount = indexCount_;
	renderer->drawState.startIndexLocation = firstIndex_;
	renderer->drawState.baseVertexLocation = baseVertexLocation_;
	renderer->drawState.hasIndexBuffer = 0 != indexBufferView_.BufferLocation;
}

void RenderContext::BindMaterial(const Material* material_, const ColorRGBA* overrideColor_)
{
	if (nullptr == renderer || nullptr == material_)
	{
		return;
	}

	renderer->drawState.materialDescriptorTable = material_->GetDescriptorTable();
	renderer->drawState.materialColor = nullptr != overrideColor_ ? *overrideColor_ : material_->GetColor();
	renderer->drawState.materialId = material_->GetId();
}

void RenderContext::SetModelMatrix(const Matrix4x4& modelMatrix_)
{
	if (nullptr == renderer)
	{
		return;
	}

	renderer->drawState.worldTransform = modelMatrix_;
}

void RenderContext::DrawArrays()
{
	if (nullptr == renderer)
	{
		return;
	}

	Renderer::GameObjectCommand command{ renderer->drawState };
	command.indexed = false;
	command.hasIndexBuffer = false;
	command.indexCount = 0;
	command.startIndexLocation = 0;
	command.baseVertexLocation = 0;
	command.sortKey = renderer->BuildSortKey(command);
	if (!renderer->IsValidCommand(command))
	{
		return;
	}

	renderer->gameObjectCommands.push_back(command);
}

void RenderContext::DrawElements()
{
	if (nullptr == renderer)
	{
		return;
	}

	Renderer::GameObjectCommand command{ renderer->drawState };
	command.indexed = command.hasIndexBuffer && command.indexCount > 0;
	command.sortKey = renderer->BuildSortKey(command);
	if (!renderer->IsValidCommand(command))
	{
		return;
	}

	renderer->gameObjectCommands.push_back(command);
}

void RenderContext::DrawUIArrays()
{
	if (nullptr == renderer)
	{
		return;
	}

	Renderer::UIObjectCommand command{};
	static_cast<Renderer::GameObjectCommand&>(command) = renderer->drawState;
	command.indexed = false;
	command.hasIndexBuffer = false;
	command.indexCount = 0;
	command.startIndexLocation = 0;
	command.baseVertexLocation = 0;
	command.sortKey = renderer->BuildSortKey(command);
	if (!renderer->IsValidCommand(command))
	{
		return;
	}

	renderer->uiObjectCommands.push_back(command);
}

void RenderContext::DrawUIElements()
{
	if (nullptr == renderer)
	{
		return;
	}

	Renderer::UIObjectCommand command{};
	static_cast<Renderer::GameObjectCommand&>(command) = renderer->drawState;
	command.indexed = command.hasIndexBuffer && command.indexCount > 0;
	command.sortKey = renderer->BuildSortKey(command);
	if (!renderer->IsValidCommand(command))
	{
		return;
	}

	renderer->uiObjectCommands.push_back(command);
}
