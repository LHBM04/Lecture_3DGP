#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstring>
#include <expected>
#include <filesystem>
#include <fstream>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "Serializer.hpp"

namespace TUK::Framework
{
	class BinaryReader final
	{
	public:
		explicit BinaryReader(std::vector<byte>&& bytes) noexcept;
		[[nodiscard]] static std::expected<BinaryReader, std::string> Open(const std::filesystem::path& path);
		template <BinaryNumber T> bool Read(T& value);
		bool Read(bool& value);
		bool Read(std::string& value);
		template <class T, usize N> bool Read(std::array<T, N>& values);
		template <class T> bool Read(std::vector<T>& values);
		template <class T> requires requires(T& value, BinaryReader& reader) { value.Deserialize(reader); }
		bool Read(T& value);
		template <class T, class... TRest> requires (sizeof...(TRest) > 0)
		bool Read(T& first, TRest&... rest);
		[[nodiscard]] bool IsAtEnd() const noexcept;
		[[nodiscard]] bool IsLegacy() const noexcept;

	private:
		std::vector<byte> bytes;
		usize position = 0;
		usize allocationBudget = SerializationLimits::MAX_BYTES;
		u32 depth = 0;
	};

	inline BinaryReader::BinaryReader(std::vector<byte>&& bytes) noexcept
		: bytes(std::move(bytes))
	{
	}

	inline std::expected<BinaryReader, std::string> BinaryReader::Open(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file)
		{
			return std::unexpected("Cannot open resource: " + path.generic_string());
		}

		const auto size = file.tellg();
		if (size <= 0 || size > static_cast<std::streamoff>(SerializationLimits::MAX_BYTES))
		{
			return std::unexpected("Invalid resource size: " + path.generic_string());
		}

		std::vector<byte> bytes(static_cast<usize>(size));
		file.seekg(0);
		if (!file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size())))
		{
			return std::unexpected("Cannot read resource: " + path.generic_string());
		}

		return BinaryReader(std::move(bytes));
	}

	template <BinaryNumber T>
	bool BinaryReader::Read(T& value)
	{
		if (sizeof(T) > bytes.size() - position)
		{
			return false;
		}

		std::array<byte, sizeof(T)> encoded{};
		std::memcpy(encoded.data(), bytes.data() + position, sizeof(T));
		position += sizeof(T);
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

		if (!complete || length > SerializationLimits::MAX_ELEMENTS || length > bytes.size() - position
			|| length > allocationBudget)
		{
			return false;
		}
		allocationBudget -= length;
		value.assign(reinterpret_cast<const char*>(bytes.data() + position), length);
		position += length;
		return Serializer::IsValidText(value);
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

	template <class T>
	bool BinaryReader::Read(std::vector<T>& values)
	{
		i32 count = 0;
		if (!Read(count) || count < 0 || static_cast<u32>(count) > SerializationLimits::MAX_ELEMENTS
			|| static_cast<usize>(count) > bytes.size() - position
			|| static_cast<usize>(count) > allocationBudget / sizeof(T))
		{
			return false;
		}

		allocationBudget -= static_cast<usize>(count) * sizeof(T);
		values.resize(count);
		for (T& value : values)
		{
			if (!Read(value))
			{
				return false;
			}
		}
		return true;
	}

	template <class T> requires requires(T& value, BinaryReader& reader) { value.Deserialize(reader); }
	bool BinaryReader::Read(T& value)
	{
		if (depth >= SerializationLimits::MAX_DEPTH)
		{
			return false;
		}
		++depth;
		const bool result = value.Deserialize(*this);
		--depth;
		return result;
	}

	template <class T, class... TRest> requires (sizeof...(TRest) > 0)
	bool BinaryReader::Read(T& first, TRest&... rest)
	{
		return Read(first) && (Read(rest) && ...);
	}

	inline bool BinaryReader::IsAtEnd() const noexcept
	{
		return position == bytes.size();
	}

	inline bool BinaryReader::IsLegacy() const noexcept
	{
		return bytes.size() >= 4 && bytes[0] == byte{ 'T' } && bytes[1] == byte{ 'U' }
			&& bytes[2] == byte{ 'K' } && bytes[3] == byte{ 'R' };
	}
}
