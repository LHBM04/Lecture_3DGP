#include "Precompiled.hpp"
#include "TextView.hpp"

#include "../Core/Common.hpp"

#include "../Objects/GameObject.hpp"
#include "../Objects/Transform.hpp"

#include "../Graphics/GPUCommandBuffer.hpp"
#include "../Graphics/GPUConstants.hpp"

#include "../Resources/BitmapFontAsset.hpp"
#include "../Resources/MaterialAsset.hpp"
#include "../Resources/TextureAsset.hpp"

namespace TUK::Framework
{
	namespace
	{
		[[nodiscard]] u32 DecodeUTF8(std::string_view text, usize& index) noexcept
		{
			const u8 lead = static_cast<u8>(text[index]);
			if (lead < 0x80u)
			{
				++index;
				return lead;
			}

			if ((lead & 0xE0u) == 0xC0u && index + 1 < text.size())
			{
				const u32 codePoint =
					((static_cast<u32>(lead) & 0x1Fu) << 6) |
					(static_cast<u32>(static_cast<u8>(text[index + 1])) & 0x3Fu);
				index += 2;
				return codePoint;
			}

			if ((lead & 0xF0u) == 0xE0u && index + 2 < text.size())
			{
				const u32 codePoint =
					((static_cast<u32>(lead) & 0x0Fu) << 12) |
					((static_cast<u32>(static_cast<u8>(text[index + 1])) & 0x3Fu) << 6) |
					(static_cast<u32>(static_cast<u8>(text[index + 2])) & 0x3Fu);
				index += 3;
				return codePoint;
			}

			if ((lead & 0xF8u) == 0xF0u && index + 3 < text.size())
			{
				const u32 codePoint =
					((static_cast<u32>(lead) & 0x07u) << 18) |
					((static_cast<u32>(static_cast<u8>(text[index + 1])) & 0x3Fu) << 12) |
					((static_cast<u32>(static_cast<u8>(text[index + 2])) & 0x3Fu) << 6) |
					(static_cast<u32>(static_cast<u8>(text[index + 3])) & 0x3Fu);
				index += 4;
				return codePoint;
			}

			++index;
			return lead;
		}
	}

	void TextView::SetFont(const BitmapFontAsset* value) noexcept
	{
		font = value;
		characterMap.clear();

		if (!font)
		{
			return;
		}

		const auto characters = font->GetCharacters();
		characterMap.reserve(characters.size());

		for (const auto& character : characters)
		{
			characterMap.emplace(character.unicode, &character);
		}
	}

	const FontCharacterData* TextView::GetCharacter(u32 unicode) const noexcept
	{
		const auto found = characterMap.find(unicode);
		if (found == characterMap.end())
		{
			return nullptr;
		}
		return found->second;
	}

	void TextView::OnPostRender(GPUCommandBuffer& commandBuffer)
	{
		if (!texture || !font || !material || text.empty())
		{
			return;
		}

		const Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			LOG_ERROR("UI 텍스트 엔티티에 Transform 컴포넌트가 필요합니다.");
			return;
		}

		ObjectConstants objectConstants
		{
			.world = transform->GetWorldMatrix()
		};

		const f32 scaleX = std::hypot(objectConstants.world[0], objectConstants.world[1]);
		const f32 scaleY = std::hypot(objectConstants.world[4], objectConstants.world[5]);
		constexpr f32 MIN_SCALE = 1.0e-6f;
		if (scaleX > MIN_SCALE)
		{
			objectConstants.world[0] /= scaleX;
			objectConstants.world[1] /= scaleX;
		}
		if (scaleY > MIN_SCALE)
		{
			objectConstants.world[4] /= scaleY;
			objectConstants.world[5] /= scaleY;
		}

		if (auto result = commandBuffer.SetGraphicsConstants(objectConstants); !result)
		{
			LOG_ERROR("UI 텍스트 변환을 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const std::array<const GPUTexture2D*, GPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{ &texture->GetTexture() };
		if (auto result = commandBuffer.BindTextures(textures); !result)
		{
			LOG_ERROR("UI 텍스트 텍스처를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		commandBuffer.SetGPUGraphicsPipelineState(material->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(material->GetPrimitive());

		const D3D12_VIEWPORT& viewport = commandBuffer.GetActiveViewport();
		const Vector2D<f32> viewportSize = (viewport.Width > 0.0f && viewport.Height > 0.0f)
			? Vector2D<f32>{ viewport.Width, viewport.Height }
			: Vector2D<f32>{ 1280.0f, 960.0f };

		const f32 pointSize = font->GetPointSize();
		const f32 scale = (pointSize > 0.0f) ? (fontSize / pointSize) : 1.0f;
		const f32 lineHeight = font->GetLineHeight() * scale;

		const auto getLineStartX = [this, scale, scaleX](usize startIndex) -> f32
		{
			if (horizontalAlignment == TextHorizontalAlignment::LEFT)
			{
				return -scaleX * 0.5f;
			}

			f32 lineWidth = 0.0f;
			while (startIndex < text.size())
			{
				const u32 codePoint = DecodeUTF8(text, startIndex);
				if (codePoint == U'\n')
				{
					break;
				}
				if (codePoint == U'\r')
				{
					continue;
				}
				if (const FontCharacterData* character = GetCharacter(codePoint))
				{
					lineWidth += character->advance * scale;
				}
			}

			return horizontalAlignment == TextHorizontalAlignment::CENTER
				? -lineWidth * 0.5f
				: scaleX * 0.5f - lineWidth;
		};
		f32 cursorX = getLineStartX(0);
		f32 baselineY = -scaleY * 0.5f + font->GetAscentLine() * scale;

		constexpr u32 TEXT_VERTEX_COUNT = 4;

		usize index = 0;
		while (index < text.size())
		{
			const u32 codePoint = DecodeUTF8(text, index);
			if (codePoint == U'\r')
			{
				continue;
			}
			if (codePoint == U'\n')
			{
				cursorX = getLineStartX(index);
				baselineY += lineHeight;
				continue;
			}

			const FontCharacterData* character = GetCharacter(codePoint);
			if (!character)
			{
				continue;
			}

			const f32 glyphWidth = character->width * scale;
			const f32 glyphHeight = character->height * scale;
			if (glyphWidth > 0.0f && glyphHeight > 0.0f)
			{
				const f32 glyphX = cursorX + character->bearingX * scale;
				const f32 glyphY = baselineY - character->bearingY * scale;

				const TextUIConstants constants
				{
					.viewportSize = viewportSize,
					.glyphRect = Vector4D<f32>{ glyphX, glyphY, glyphWidth, glyphHeight },
					.glyphUV = Vector4D<f32>{ character->uvMin[0], character->uvMin[1], character->uvMax[0], character->uvMax[1] },
					.textColor = color
				};

				if (auto result = commandBuffer.SetGraphicsConstants(constants); !result)
				{
					LOG_ERROR("UI 텍스트 상수를 바인딩하지 못했습니다: {}", result.error());
					return;
				}

				commandBuffer.Draw(TEXT_VERTEX_COUNT);
			}

			cursorX += character->advance * scale;
		}
	}
}
