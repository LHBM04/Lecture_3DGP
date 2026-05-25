#include "Precompiled.h"
#include "ImageView.h"

#include "Application.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "RectTransform.h"
#include "InputManager.h"
#include "Renderer.h"
#include "Shader.h"
#include "Vector2D.h"
#include "DrawCall.h"

namespace
{
	Matrix4x4 BuildClipSpaceRectMatrix(int x_, int y_, int width_, int height_, int screenWidth_, int screenHeight_) noexcept
	{
		const float left{ (static_cast<float>(x_) / static_cast<float>(screenWidth_)) * 2.0f - 1.0f };
		const float right{ (static_cast<float>(x_ + width_) / static_cast<float>(screenWidth_)) * 2.0f - 1.0f };
		const float top{ 1.0f - (static_cast<float>(y_) / static_cast<float>(screenHeight_)) * 2.0f };
		const float bottom{ 1.0f - (static_cast<float>(y_ + height_) / static_cast<float>(screenHeight_)) * 2.0f };

		return Matrix4x4(
			right - left, 0.0f, 0.0f, 0.0f,
			0.0f, bottom - top, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			left, top, 0.0f, 1.0f);
	}
}

Mesh* ImageView::GetMesh() const noexcept
{
	return mesh;
}

void ImageView::SetMesh(Mesh* mesh_) noexcept
{
	mesh = mesh_;
}

Material* ImageView::GetMaterial() const noexcept
{
	return material;
}

void ImageView::SetMaterial(Material* material_) noexcept
{
	material = material_;
}

const ColorRGBA& ImageView::GetColor() const noexcept
{
	return color;
}

void ImageView::SetColor(const ColorRGBA& color_) noexcept
{
	color = color_;
}

void ImageView::OnAttach()
{
	GameObject* owner{ GetOwner() };
	if (nullptr == owner)
	{
		return;
	}

	if (nullptr == owner->GetComponent<RectTransform>())
	{
		owner->AddComponent<RectTransform>();
	}
}

void ImageView::OnRender()
{
	GameObject* owner{ GetOwner() };
	if (nullptr == owner)
	{
		return;
	}

	if (RectTransform* rectTransform{ owner->GetComponent<RectTransform>() })
	{
		if (nullptr == material || nullptr == mesh)
		{
			return;
		}

		Shader* shader{ material->GetShader() };
		if (nullptr == shader)
		{
			return;
		}

		const auto [screenWidth, screenHeight]{ InputManager::GetScreenSize() };
		const Vector2D& anchoredPosition{ rectTransform->GetAnchoredPosition() };
		const Vector2D& pivot{ rectTransform->GetPivot() };
		const Vector2D& rectSize{ rectTransform->GetSize() };

		const int x{ static_cast<int>(screenWidth * 0.5f + anchoredPosition.x - rectSize.x * pivot.x) };
		const int y{ static_cast<int>(screenHeight * 0.5f - anchoredPosition.y - rectSize.y * pivot.y) };

		Renderer& renderer{ Application::GetRenderer() };
		DrawCall drawCall{};
		drawCall.pipelineState = shader->GetPipelineState();
		drawCall.graphicsRootSignature = shader->GetGraphicsRootSignature();
		drawCall.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		drawCall.vertexBufferView = mesh->GetVertexBufferView();
		drawCall.indexBufferView = mesh->GetIndexBufferView();
		drawCall.hasIndexBuffer = mesh->HasIndexBuffer();
		drawCall.indexed = mesh->HasIndexBuffer();
		drawCall.materialDescriptorTable = material->GetDescriptorTable();
		drawCall.materialColor = color;
		drawCall.vertexCount = mesh->GetVertexCount();
		drawCall.startVertexLocation = 0;
		drawCall.indexCount = mesh->GetIndexCount();
		drawCall.startIndexLocation = 0;
		drawCall.baseVertexLocation = 0;
		drawCall.instanceCount = 1;
		drawCall.startInstanceLocation = 0;
		drawCall.pipelineId = shader->GetPipelineId();
		drawCall.materialId = material->GetId();
		drawCall.meshId = mesh->GetId();
		drawCall.worldTransform = BuildClipSpaceRectMatrix(
			x,
			y,
			static_cast<int>(rectSize.x),
			static_cast<int>(rectSize.y),
			std::max(1, screenWidth),
			std::max(1, screenHeight));
		drawCall.sortKey =
			((drawCall.pipelineId & 0xFFFFull) << 48) |
			((drawCall.materialId & 0xFFFFFFull) << 24) |
			(drawCall.meshId & 0xFFFFFFull);
		renderer.SubmitDrawCall(drawCall);

		return;
	}

	if (nullptr == material || nullptr == mesh)
	{
		return;
	}

	MeshRenderer* renderer{ owner->GetComponent<MeshRenderer>() };
	if (nullptr == renderer)
	{
		return;
	}

	material->SetColor(color);
	renderer->SetMesh(mesh);
	renderer->SetMaterial(material);
}
