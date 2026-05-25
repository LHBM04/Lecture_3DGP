#include "Precompiled.h"
#include "TextView.h"

#include "Application.h"
#include "GameObject.h"
#include "Font.h"
#include "InputManager.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "RectTransform.h"
#include "Renderer.h"
#include "DrawCall.h"
#include "Shader.h"

namespace
{
	const char* GetGlyphRows(wchar_t character_) noexcept
	{
		switch (::towupper(character_))
		{
		case L'A': return "01110" "10001" "10001" "11111" "10001" "10001" "10001";
		case L'B': return "11110" "10001" "10001" "11110" "10001" "10001" "11110";
		case L'C': return "01111" "10000" "10000" "10000" "10000" "10000" "01111";
		case L'D': return "11110" "10001" "10001" "10001" "10001" "10001" "11110";
		case L'E': return "11111" "10000" "10000" "11110" "10000" "10000" "11111";
		case L'F': return "11111" "10000" "10000" "11110" "10000" "10000" "10000";
		case L'G': return "01111" "10000" "10000" "10111" "10001" "10001" "01111";
		case L'H': return "10001" "10001" "10001" "11111" "10001" "10001" "10001";
		case L'I': return "11111" "00100" "00100" "00100" "00100" "00100" "11111";
		case L'J': return "00111" "00010" "00010" "00010" "10010" "10010" "01100";
		case L'K': return "10001" "10010" "10100" "11000" "10100" "10010" "10001";
		case L'L': return "10000" "10000" "10000" "10000" "10000" "10000" "11111";
		case L'M': return "10001" "11011" "10101" "10101" "10001" "10001" "10001";
		case L'N': return "10001" "11001" "10101" "10011" "10001" "10001" "10001";
		case L'O': return "01110" "10001" "10001" "10001" "10001" "10001" "01110";
		case L'P': return "11110" "10001" "10001" "11110" "10000" "10000" "10000";
		case L'Q': return "01110" "10001" "10001" "10001" "10101" "10010" "01101";
		case L'R': return "11110" "10001" "10001" "11110" "10100" "10010" "10001";
		case L'S': return "01111" "10000" "10000" "01110" "00001" "00001" "11110";
		case L'T': return "11111" "00100" "00100" "00100" "00100" "00100" "00100";
		case L'U': return "10001" "10001" "10001" "10001" "10001" "10001" "01110";
		case L'V': return "10001" "10001" "10001" "10001" "10001" "01010" "00100";
		case L'W': return "10001" "10001" "10001" "10101" "10101" "10101" "01010";
		case L'X': return "10001" "10001" "01010" "00100" "01010" "10001" "10001";
		case L'Y': return "10001" "10001" "01010" "00100" "00100" "00100" "00100";
		case L'Z': return "11111" "00001" "00010" "00100" "01000" "10000" "11111";
		case L'0': return "01110" "10001" "10011" "10101" "11001" "10001" "01110";
		case L'1': return "00100" "01100" "00100" "00100" "00100" "00100" "01110";
		case L'2': return "01110" "10001" "00001" "00010" "00100" "01000" "11111";
		case L'3': return "11110" "00001" "00001" "01110" "00001" "00001" "11110";
		case L'4': return "00010" "00110" "01010" "10010" "11111" "00010" "00010";
		case L'5': return "11111" "10000" "10000" "11110" "00001" "00001" "11110";
		case L'6': return "01110" "10000" "10000" "11110" "10001" "10001" "01110";
		case L'7': return "11111" "00001" "00010" "00100" "01000" "01000" "01000";
		case L'8': return "01110" "10001" "10001" "01110" "10001" "10001" "01110";
		case L'9': return "01110" "10001" "10001" "01111" "00001" "00001" "01110";
		default: return nullptr;
		}
	}

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

	uint64_t BuildSortKey(uint64_t pipelineId_, uint64_t materialId_, uint64_t meshId_) noexcept
	{
		return ((pipelineId_ & 0xFFFFull) << 48) |
			((materialId_ & 0xFFFFFFull) << 24) |
			(meshId_ & 0xFFFFFFull);
	}
}

const std::wstring& TextView::GetText() const noexcept
{
	return text;
}

void TextView::SetText(const std::wstring& text_) noexcept
{
	text = text_;
}

Font* TextView::GetFont() const noexcept
{
	return font;
}

void TextView::SetFont(Font* font_) noexcept
{
	font = font_;
	hasLoggedMissingFont = false;
}

Mesh* TextView::GetMesh() const noexcept
{
	return mesh;
}

void TextView::SetMesh(Mesh* mesh_) noexcept
{
	mesh = mesh_;
}

Material* TextView::GetMaterial() const noexcept
{
	return material;
}

void TextView::SetMaterial(Material* material_) noexcept
{
	material = material_;
}

const ColorRGBA& TextView::GetColor() const noexcept
{
	return color;
}

void TextView::SetColor(const ColorRGBA& color_) noexcept
{
	color = color_;
}

void TextView::OnAttach()
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

void TextView::OnRender()
{
	GameObject* owner{ GetOwner() };
	if (nullptr == owner)
	{
		return;
	}

	RectTransform* rectTransform{ owner->GetComponent<RectTransform>() };
	if (nullptr == rectTransform)
	{
		return;
	}
	if (nullptr == font)
	{
		if (!hasLoggedMissingFont)
		{
			Logger::Warning("TextView font is not assigned.");
			hasLoggedMissingFont = true;
		}
		return;
	}

	if (!font->IsLoaded())
	{
		if (!hasLoggedMissingFont)
		{
			Logger::Warning("TextView font is not loaded. path=%s", font->GetPath().string().c_str());
			hasLoggedMissingFont = true;
		}
		return;
	}
	if (nullptr == mesh || nullptr == material)
	{
		return;
	}

	Shader* shader{ material->GetShader() };
	if (nullptr == shader)
	{
		return;
	}

	hasLoggedMissingFont = false;
	Renderer& renderer{ Application::GetRenderer() };

	const auto [screenWidth, screenHeight]{ InputManager::GetScreenSize() };
	const Vector2D& anchoredPosition{ rectTransform->GetAnchoredPosition() };
	const Vector2D& pivot{ rectTransform->GetPivot() };
	const Vector2D& size{ rectTransform->GetSize() };

	const int fontSize{ font->GetSize() };
	const int rectX{ static_cast<int>(screenWidth * 0.5f + anchoredPosition.x - size.x * pivot.x) };
	const int rectY{ static_cast<int>(screenHeight * 0.5f - anchoredPosition.y - size.y * pivot.y) };
	const int x{ rectX };
	const int y{ rectY + static_cast<int>(std::max(0.0f, (size.y - static_cast<float>(fontSize)) * 0.5f)) };

	const int pixelScale{ std::max(1, fontSize / 7) };
	const int glyphAdvance{ pixelScale * 6 };

	int cursorX{ x };
	int cursorY{ y };

	for (wchar_t character : text)
	{
		if (L'\n' == character)
		{
			cursorX = x;
			cursorY += pixelScale * 8;
			continue;
		}

		if (L' ' == character)
		{
			cursorX += glyphAdvance;
			continue;
		}

		const char* glyphRows{ GetGlyphRows(character) };
		if (nullptr == glyphRows)
		{
			cursorX += glyphAdvance;
			continue;
		}

		for (int row{ 0 }; row < 7; ++row)
		{
			for (int column{ 0 }; column < 5; ++column)
			{
				if ('1' != glyphRows[(row * 5) + column])
				{
					continue;
				}

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
					cursorX + (column * pixelScale),
					cursorY + (row * pixelScale),
					pixelScale,
					pixelScale,
					std::max(1, screenWidth),
					std::max(1, screenHeight));
				drawCall.sortKey = BuildSortKey(drawCall.pipelineId, drawCall.materialId, drawCall.meshId);

				renderer.SubmitDrawCall(drawCall);
			}
		}

		cursorX += glyphAdvance;
	}
}
