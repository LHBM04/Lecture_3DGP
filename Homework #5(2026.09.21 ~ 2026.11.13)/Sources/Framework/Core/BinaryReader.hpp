#pragma once

#include <bit>

#include "Common.hpp"

namespace TUK::Framework
{
	class BinaryReader final
	{
	public:
		static constexpr usize MAX_BYTES = 512 * 1024 * 1024;
		static constexpr u32 MAX_ELEMENTS = 16 * 1024 * 1024;
		static constexpr u32 MAX_DEPTH = 128;

		explicit BinaryReader(std::ifstream& file) noexcept;
		~BinaryReader() noexcept = default;

		BinaryReader(const BinaryReader&) = delete;
		BinaryReader& operator=(const BinaryReader&) = delete;

		BinaryReader(BinaryReader&&) = delete;
		BinaryReader& operator=(BinaryReader&&) = delete;

		template <class T>
		requires ((std::integral<T> && !std::same_as<T, bool>) || std::floating_point<T>)
		[[nodiscard]] bool Read(T& value);

		[[nodiscard]] bool Read(bool& value);

		[[nodiscard]] bool Read(std::string& value);

		template <class T, usize N>
		[[nodiscard]] bool Read(std::array<T, N>& values);

		template <class T, class... TRest>
		[[nodiscard]] bool Read(T& first, TRest&... rest);

		[[nodiscard]] bool ReadCount(i32& count, usize elementSize);

		[[nodiscard]] bool IsAtEnd() const noexcept;

	private:
		[[nodiscard]] bool ReadBytes(char* destination, usize size);
		[[nodiscard]] static bool IsValidText(std::string_view text) noexcept;

		std::ifstream& file;
		usize remainingBytes = 0;
		usize allocationBudget = MAX_BYTES;
	};

	inline BinaryReader::BinaryReader(std::ifstream& file) noexcept
		: file(file)
	{
		if (!file)
		{
			return;
		}

		file.seekg(0, std::ios::end);
		const auto size = file.tellg();
		if (size <= 0 || size > static_cast<std::streamoff>(MAX_BYTES))
		{
			file.setstate(std::ios::failbit);
			return;
		}

		remainingBytes = static_cast<usize>(size);
		file.seekg(0);
	}

	template <class T>
	requires ((std::integral<T> && !std::same_as<T, bool>) || std::floating_point<T>)
	bool BinaryReader::Read(T& value)
	{
		std::array<char, sizeof(T)> encoded{};
		if (!ReadBytes(encoded.data(), encoded.size()))
		{
			return false;
		}

		if constexpr (std::endian::native == std::endian::big)
		{
			std::ranges::reverse(encoded);
		}

		value = std::bit_cast<T>(encoded);
		if constexpr (std::floating_point<T>)
		{
			return std::isfinite(value);
		}

		return true;
	}

	inline bool BinaryReader::Read(bool& value)
	{
		u8 encoded = 0;
		if (!Read(encoded) || encoded > 1)
		{
			return false;
		}

		value = encoded != 0;
		return true;
	}

	inline bool BinaryReader::Read(std::string& value)
	{
		u32 length = 0;
		bool complete = false;
		for (u32 i = 0; i < 5; ++i)
		{
			u8 encoded = 0;
			if (!Read(encoded) || (i == 4 && encoded > 7))
			{
				return false;
			}

			length |= static_cast<u32>(encoded & 0x7F) << (i * 7);
			if ((encoded & 0x80) == 0)
			{
				complete = true;
				break;
			}
		}

		if (!complete || length > MAX_ELEMENTS || length > remainingBytes || length > allocationBudget)
		{
			return false;
		}

		allocationBudget -= length;
		value.resize(length);

		return ReadBytes(value.data(), length) && IsValidText(value);
	}

	template <class T, usize N>
	bool BinaryReader::Read(std::array<T, N>& values)
	{
		for (T& value : values)
		{
			if (!Read(value))
			{
				return false;
			}
		}

		return true;
	}

	template <class T, class... TRest>
	bool BinaryReader::Read(T& first, TRest&... rest)
	{
		return Read(first) && (Read(rest) && ...);
	}

	inline bool BinaryReader::ReadCount(i32& count, usize elementSize)
	{
		if (!Read(count) || count < 0 || static_cast<u32>(count) > MAX_ELEMENTS ||
			static_cast<usize>(count) > remainingBytes || elementSize == 0 ||
			static_cast<usize>(count) > allocationBudget / elementSize)
		{
			return false;
		}

		allocationBudget -= static_cast<usize>(count) * elementSize;
		return true;
	}

	inline bool BinaryReader::IsAtEnd() const noexcept
	{
		return file.good() && remainingBytes == 0;
	}

	inline bool BinaryReader::ReadBytes(char* destination, usize size)
	{
		if (!file || size > remainingBytes || !file.read(destination, static_cast<std::streamsize>(size)))
		{
			return false;
		}

		remainingBytes -= size;
		return true;
	}

	inline bool BinaryReader::IsValidText(std::string_view text) noexcept
	{
		for (usize i = 0; i < text.size();)
		{
			const auto first = static_cast<u8>(text[i++]);
			if (first == 0)
			{
				return false;
			}

			if (first < 0x80)
			{
				continue;
			}

			u32 codepoint = 0;
			u32 minimum = 0;
			usize continuationCount = 0;
			if (first >= 0xC2 && first <= 0xDF)
			{
				codepoint = first & 0x1F;
				minimum = 0x80;
				continuationCount = 1;
			}
			else if (first >= 0xE0 && first <= 0xEF)
			{
				codepoint = first & 0x0F;
				minimum = 0x800;
				continuationCount = 2;
			}
			else if (first >= 0xF0 && first <= 0xF4)
			{
				codepoint = first & 0x07;
				minimum = 0x10000;
				continuationCount = 3;
			}
			else
			{
				return false;
			}

			if (continuationCount > text.size() - i)
			{
				return false;
			}

			for (usize j = 0; j < continuationCount; ++j)
			{
				const auto next = static_cast<u8>(text[i++]);
				if ((next & 0xC0) != 0x80)
				{
					return false;
				}

				codepoint = (codepoint << 6) | (next & 0x3F);
			}

			if (codepoint < minimum || codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF))
			{
				return false;
			}
		}

		return true;
	}
}
