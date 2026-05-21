#pragma once

#include <array>
#include <cstdint>
#include <unordered_map>

#include "Asset.h"

struct FontGlyph final
{
	static constexpr int width{ 5 };
	static constexpr int height{ 7 };

	std::array<std::uint8_t, height> rows{};
};

class Font final : public Asset
{
public:
	Font() noexcept;
	virtual ~Font() noexcept override = default;

	virtual bool Load(const std::filesystem::path& path_) override;
	virtual void Unload() noexcept override;

	[[nodiscard]] const FontGlyph& GetGlyph(char character_) const noexcept;
	[[nodiscard]] int GetGlyphWidth() const noexcept;
	[[nodiscard]] int GetGlyphHeight() const noexcept;

private:
	void LoadBuiltinGlyphs() noexcept;

	std::unordered_map<char, FontGlyph> glyphs;
	FontGlyph blankGlyph{};
};
