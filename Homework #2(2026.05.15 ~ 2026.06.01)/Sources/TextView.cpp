#include "Precompiled.h"
#include "TextView.h"

#include "GameObject.h"
#include "Font.h"
#include "InputManager.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "RectTransform.h"
#include "RenderContext.h"
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

}

const std::wstring& TextView::GetText() const noexcept
{
	return text;
}

void TextView::SetText(const std::wstring& text_) noexcept
{
	text = text_;
	MarkDirty();
}

Font* TextView::GetFont() const noexcept
{
	return font;
}

void TextView::SetFont(Font* font_) noexcept
{
	font = font_;
	hasLoggedMissingFont = false;
	MarkDirty();
}

Mesh* TextView::GetMesh() const noexcept
{
	return mesh;
}

void TextView::SetMesh(Mesh* mesh_) noexcept
{
	mesh = mesh_;
	MarkDirty();
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

	if (nullptr == cachedMesh)
	{
		cachedMesh = std::make_unique<Mesh>();
	}
	MarkDirty();
}

void TextView::OnRenderUI(RenderContext& context_)
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
	if (nullptr == material)
	{
		return;
	}

	hasLoggedMissingFont = false;
	RebuildMeshIfDirty(*rectTransform);

	if (nullptr == cachedMesh || 0 == cachedMesh->GetVertexCount())
	{
		return;
	}

	Shader* shader{ material->GetShader() };
	if (nullptr == shader)
	{
		return;
	}

	context_.UseProgram(shader);
	context_.BindVertexBuffer(cachedMesh->GetVertexBufferView(), cachedMesh->GetVertexCount(), cachedMesh->GetId());
	if (cachedMesh->HasIndexBuffer())
	{
		context_.BindElementBuffer(cachedMesh->GetIndexBufferView(), cachedMesh->GetIndexCount());
	}
	context_.BindMaterial(material, &color);
	context_.SetModelMatrix(Matrix4x4::GetIdentity());

	if (cachedMesh->HasIndexBuffer())
	{
		context_.DrawUIElements();
	}
	else
	{
		context_.DrawUIArrays();
	}
}

void TextView::MarkDirty() noexcept
{
	isDirty = true;
}

void TextView::RebuildMeshIfDirty(RectTransform& rectTransform_)
{
	if (nullptr == cachedMesh || nullptr == font)
	{
		return;
	}

	const auto [screenWidth, screenHeight]{ InputManager::GetScreenSize() };
	const Vector2D anchoredPosition{ rectTransform_.GetAnchoredPosition() };
	const Vector2D pivot{ rectTransform_.GetPivot() };
	const Vector2D size{ rectTransform_.GetSize() };
	const int fontSize{ font->GetSize() };

	const bool transformChanged{
		anchoredPosition.x != cachedAnchoredPosition.x ||
		anchoredPosition.y != cachedAnchoredPosition.y ||
		size.x != cachedSize.x ||
		size.y != cachedSize.y ||
		pivot.x != cachedPivot.x ||
		pivot.y != cachedPivot.y ||
		screenWidth != cachedScreenWidth ||
		screenHeight != cachedScreenHeight ||
		fontSize != cachedFontSize
	};

	if (!isDirty && !transformChanged)
	{
		return;
	}

	std::vector<Mesh::Vertex> vertices;
	std::vector<std::uint32_t> indices;
	vertices.reserve(text.size() * 4u * 16u);
	indices.reserve(text.size() * 6u * 16u);

	const int rectX{ static_cast<int>(screenWidth * 0.5f + anchoredPosition.x - size.x * pivot.x) };
	const int rectY{ static_cast<int>(screenHeight * 0.5f - anchoredPosition.y - size.y * pivot.y) };
	const int x{ rectX };
	const int y{ rectY + static_cast<int>(std::max(0.0f, (size.y - static_cast<float>(fontSize)) * 0.5f)) };

	const int pixelScale{ std::max(1, fontSize / 7) };
	const int glyphAdvance{ pixelScale * 6 };

	auto pixelToClipX = [screenWidth](int px_) noexcept
		{
			return (static_cast<float>(px_) / static_cast<float>(std::max(1, screenWidth))) * 2.0f - 1.0f;
		};
	auto pixelToClipY = [screenHeight](int py_) noexcept
		{
			return 1.0f - (static_cast<float>(py_) / static_cast<float>(std::max(1, screenHeight))) * 2.0f;
		};

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

				const int leftPx{ cursorX + column * pixelScale };
				const int topPx{ cursorY + row * pixelScale };
				const int rightPx{ leftPx + pixelScale };
				const int bottomPx{ topPx + pixelScale };

				const float left{ pixelToClipX(leftPx) };
				const float right{ pixelToClipX(rightPx) };
				const float top{ pixelToClipY(topPx) };
				const float bottom{ pixelToClipY(bottomPx) };

				const std::uint32_t baseIndex{ static_cast<std::uint32_t>(vertices.size()) };
				vertices.push_back(Mesh::Vertex{ { left,  top,    0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } });
				vertices.push_back(Mesh::Vertex{ { right, top,    0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } });
				vertices.push_back(Mesh::Vertex{ { right, bottom, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } });
				vertices.push_back(Mesh::Vertex{ { left,  bottom, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } });

				indices.push_back(baseIndex + 0u);
				indices.push_back(baseIndex + 1u);
				indices.push_back(baseIndex + 2u);
				indices.push_back(baseIndex + 0u);
				indices.push_back(baseIndex + 2u);
				indices.push_back(baseIndex + 3u);
			}
		}

		cursorX += glyphAdvance;
	}

	if (vertices.empty())
	{
		cachedMesh->Unload();
	}
	else
	{
		if (cachedMesh->BuildFromRaw(vertices, indices))
		{
			const std::size_t hashValue{ std::hash<const TextView*>{}(this) };
			cachedMesh->SetId(static_cast<uint64_t>(0x10000000ull | (hashValue & 0x0FFFFFFFull)));
		}
	}

	cachedAnchoredPosition = anchoredPosition;
	cachedSize = size;
	cachedPivot = pivot;
	cachedScreenWidth = screenWidth;
	cachedScreenHeight = screenHeight;
	cachedFontSize = fontSize;
	isDirty = false;
}
