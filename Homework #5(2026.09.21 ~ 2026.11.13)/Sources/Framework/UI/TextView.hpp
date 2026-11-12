#pragma once

#include "../Core/Common.hpp"

#include "../Math/ColorRGBA.hpp"

#include "../Objects/Component.hpp"

namespace TUK::Framework
{
	class BitmapFontAsset;
	class MaterialAsset;
	class TextureAsset;
	struct FontCharacterData;

	class TextView final : public Component
	{
		using Component::Component;

	public:
		void OnPostRender(GPUCommandBuffer& commandBuffer) override;

		[[nodiscard]] const TextureAsset* GetTexture() const noexcept;
		void SetTexture(const TextureAsset* value) noexcept;

		[[nodiscard]] const BitmapFontAsset* GetFont() const noexcept;
		void SetFont(const BitmapFontAsset* value) noexcept;

		[[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
		void SetMaterial(const MaterialAsset* value) noexcept;

		[[nodiscard]] const std::string& GetText() const noexcept;
		void SetText(std::string value) noexcept;

		[[nodiscard]] f32 GetFontSize() const noexcept;
		void SetFontSize(f32 value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetColor() const noexcept;
		void SetColor(const ColorRGBA<f32>& value) noexcept;

	private:
		[[nodiscard]] const FontCharacterData* GetCharacter(u32 unicode) const noexcept;

		const TextureAsset* texture = nullptr;
		const BitmapFontAsset* font = nullptr;
		const MaterialAsset* material = nullptr;

		std::string text = "New Text!";
		f32 fontSize = 32.0f;
		ColorRGBA<f32> color = ColorRGBA<f32>::GetWhite();
		std::unordered_map<u32, const FontCharacterData*> characterMap;
	};

	inline const TextureAsset* TextView::GetTexture() const noexcept
	{
		return texture;
	}

	inline void TextView::SetTexture(const TextureAsset* value) noexcept
	{
		texture = value;
	}

	inline const BitmapFontAsset* TextView::GetFont() const noexcept
	{
		return font;
	}

	inline const MaterialAsset* TextView::GetMaterial() const noexcept
	{
		return material;
	}

	inline void TextView::SetMaterial(const MaterialAsset* value) noexcept
	{
		material = value;
	}

	inline const std::string& TextView::GetText() const noexcept
	{
		return text;
	}

	inline void TextView::SetText(std::string value) noexcept
	{
		text = std::move(value);
	}

	inline f32 TextView::GetFontSize() const noexcept
	{
		return fontSize;
	}

	inline void TextView::SetFontSize(f32 value) noexcept
	{
		fontSize = value;
	}

	inline const ColorRGBA<f32>& TextView::GetColor() const noexcept
	{
		return color;
	}

	inline void TextView::SetColor(const ColorRGBA<f32>& value) noexcept
	{
		color = value;
	}
}
