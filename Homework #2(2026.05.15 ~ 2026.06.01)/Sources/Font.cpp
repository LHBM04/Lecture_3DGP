#include "Precompiled.h"
#include "Font.h"

namespace
{
	[[nodiscard]] FontGlyph MakeGlyph(
		std::uint8_t row0_,
		std::uint8_t row1_,
		std::uint8_t row2_,
		std::uint8_t row3_,
		std::uint8_t row4_,
		std::uint8_t row5_,
		std::uint8_t row6_) noexcept
	{
		return FontGlyph{ { row0_, row1_, row2_, row3_, row4_, row5_, row6_ } };
	}
}

Font::Font() noexcept
{
	LoadBuiltinGlyphs();
	SetLoaded(true);
}

bool Font::Load(const std::filesystem::path& path_)
{
	Unload();
	LoadBuiltinGlyphs();
	SetPath(path_);
	SetLoaded(true);
	return true;
}

void Font::Unload() noexcept
{
	glyphs.clear();
	SetLoaded(false);
}

const FontGlyph& Font::GetGlyph(char character_) const noexcept
{
	if ('a' <= character_ && character_ <= 'z')
	{
		character_ = static_cast<char>(character_ - 'a' + 'A');
	}

	const auto iterator{ glyphs.find(character_) };
	return iterator != glyphs.end() ? iterator->second : blankGlyph;
}

int Font::GetGlyphWidth() const noexcept
{
	return FontGlyph::width;
}

int Font::GetGlyphHeight() const noexcept
{
	return FontGlyph::height;
}

void Font::LoadBuiltinGlyphs() noexcept
{
	blankGlyph = {};
	glyphs.clear();

	glyphs.emplace('0', MakeGlyph(0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110));
	glyphs.emplace('1', MakeGlyph(0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110));
	glyphs.emplace('2', MakeGlyph(0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111));
	glyphs.emplace('3', MakeGlyph(0b11110, 0b00001, 0b00001, 0b01110, 0b00001, 0b00001, 0b11110));
	glyphs.emplace('4', MakeGlyph(0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010));
	glyphs.emplace('5', MakeGlyph(0b11111, 0b10000, 0b10000, 0b11110, 0b00001, 0b00001, 0b11110));
	glyphs.emplace('6', MakeGlyph(0b01110, 0b10000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110));
	glyphs.emplace('7', MakeGlyph(0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000));
	glyphs.emplace('8', MakeGlyph(0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110));
	glyphs.emplace('9', MakeGlyph(0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00001, 0b01110));

	glyphs.emplace('A', MakeGlyph(0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001));
	glyphs.emplace('B', MakeGlyph(0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110));
	glyphs.emplace('C', MakeGlyph(0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110));
	glyphs.emplace('D', MakeGlyph(0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110));
	glyphs.emplace('E', MakeGlyph(0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111));
	glyphs.emplace('F', MakeGlyph(0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000));
	glyphs.emplace('G', MakeGlyph(0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01111));
	glyphs.emplace('H', MakeGlyph(0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001));
	glyphs.emplace('I', MakeGlyph(0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110));
	glyphs.emplace('J', MakeGlyph(0b00111, 0b00010, 0b00010, 0b00010, 0b10010, 0b10010, 0b01100));
	glyphs.emplace('K', MakeGlyph(0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001));
	glyphs.emplace('L', MakeGlyph(0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111));
	glyphs.emplace('M', MakeGlyph(0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001));
	glyphs.emplace('N', MakeGlyph(0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001));
	glyphs.emplace('O', MakeGlyph(0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110));
	glyphs.emplace('P', MakeGlyph(0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000));
	glyphs.emplace('Q', MakeGlyph(0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101));
	glyphs.emplace('R', MakeGlyph(0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001));
	glyphs.emplace('S', MakeGlyph(0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110));
	glyphs.emplace('T', MakeGlyph(0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100));
	glyphs.emplace('U', MakeGlyph(0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110));
	glyphs.emplace('V', MakeGlyph(0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b01010, 0b00100));
	glyphs.emplace('W', MakeGlyph(0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b11011, 0b10001));
	glyphs.emplace('X', MakeGlyph(0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001));
	glyphs.emplace('Y', MakeGlyph(0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100));
	glyphs.emplace('Z', MakeGlyph(0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111));
}
