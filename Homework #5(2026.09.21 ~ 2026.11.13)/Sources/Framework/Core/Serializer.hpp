#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstring>
#include <expected>
#include <filesystem>
#include <fstream>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Common.hpp"

namespace TUK::Framework
{
	// These limits apply to both file data and aggregate decoded allocations.
	struct SerializationLimits final
	{
		static constexpr usize MAX_BYTES = 512 * 1024 * 1024;
		static constexpr u32 MAX_ELEMENTS = 16 * 1024 * 1024;
		static constexpr u32 MAX_DEPTH = 128;
		static constexpr u32 MAX_FIELDS = 1024;
	};

	template <class T>
	concept BinaryNumber = (std::integral<T> && !std::same_as<T, bool>) || std::floating_point<T>;

	class BinaryOutputArchive final
	{
	public:
		explicit BinaryOutputArchive(std::vector<byte>& bytes, u32 depth = 0) noexcept;

		template <BinaryNumber T> bool Value(T value);
		bool Value(const std::string& value);
		template <class T, usize N> bool Value(std::array<T, N>& values);
		template <class T> bool Value(std::vector<T>& values);
		template <class T> requires requires(T& value, BinaryOutputArchive& archive) { value.Serialize(archive); }
		bool Value(T& value);
		template <class T> bool Field(u32 id, T& value);
		template <class T> bool OptionalField(u32 id, T& value);
		bool PatchSize(usize offset, u64 size);

	private:
		std::vector<byte>& bytes;
		u32 depth;
		std::unordered_set<u32> fieldIDs;
	};

	class BinaryInputArchive final
	{
	public:
		explicit BinaryInputArchive(std::span<const byte> bytes, usize& allocationBudget, u32 depth = 0) noexcept;

		template <BinaryNumber T> bool Value(T& value);
		bool Value(std::string& value);
		template <class T, usize N> bool Value(std::array<T, N>& values);
		template <class T> bool Value(std::vector<T>& values);
		template <class T> requires requires(T& value, BinaryInputArchive& archive) { value.Serialize(archive); }
		bool Value(T& value);
		template <class T> bool Field(u32 id, T& value);
		template <class T> bool OptionalField(u32 id, T& value);
		[[nodiscard]] bool AtEnd() const noexcept;

	private:
		bool IndexFields();
		bool Allocate(usize count, usize elementSize) noexcept;
		std::span<const byte> bytes;
		usize& allocationBudget;
		usize position = 0;
		u32 depth;
		std::unordered_map<u32, std::span<const byte>> fields;
	};

	class Serializer final
	{
	public:
		static constexpr u32 MAGIC = 0x524B5554; // TUKR, little-endian
		static constexpr u32 FORMAT_VERSION = 1;

		[[nodiscard]] static bool IsValidText(std::string_view text) noexcept;

		template <class T>
		[[nodiscard]] static std::expected<T, std::string> Load(const std::filesystem::path& path, u32 resourceType);
		template <class T>
		[[nodiscard]] static std::expected<void, std::string> Save(
			const std::filesystem::path& path, u32 resourceType, T& value);
	};

	inline bool Serializer::IsValidText(std::string_view text) noexcept
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

	inline BinaryOutputArchive::BinaryOutputArchive(std::vector<byte>& bytes, u32 depth) noexcept
		: bytes(bytes), depth(depth)
	{
	}

	template <BinaryNumber T>
	bool BinaryOutputArchive::Value(T value)
	{
		if constexpr (std::floating_point<T>)
		{
			static_assert(std::numeric_limits<T>::is_iec559);
			if (!std::isfinite(value))
			{
				return false;
			}
		}
		if (bytes.size() > SerializationLimits::MAX_BYTES - sizeof(T))
		{
			return false;
		}
		auto encoded = std::bit_cast<std::array<byte, sizeof(T)>>(value);
		if constexpr (std::endian::native == std::endian::big)
		{
			std::ranges::reverse(encoded);
		}
		bytes.insert(bytes.end(), encoded.begin(), encoded.end());
		return true;
	}

	inline bool BinaryOutputArchive::Value(const std::string& value)
	{
		if (!Serializer::IsValidText(value) || value.size() > SerializationLimits::MAX_ELEMENTS || !Value(static_cast<u32>(value.size()))
			|| value.size() > SerializationLimits::MAX_BYTES - bytes.size())
		{
			return false;
		}
		const auto encoded = std::as_bytes(std::span(value.data(), value.size()));
		bytes.insert(bytes.end(), encoded.begin(), encoded.end());
		return true;
	}

	template <class T, usize N>
	bool BinaryOutputArchive::Value(std::array<T, N>& values)
	{
		for (auto& value : values)
		{
			if (!Value(value))
			{
				return false;
			}
		}
		return true;
	}

	template <class T>
	bool BinaryOutputArchive::Value(std::vector<T>& values)
	{
		if (values.size() > SerializationLimits::MAX_ELEMENTS || !Value(static_cast<u32>(values.size())))
		{
			return false;
		}
		for (auto& value : values)
		{
			if (!Value(value))
			{
				return false;
			}
		}
		return true;
	}

	template <class T> requires requires(T& value, BinaryOutputArchive& archive) { value.Serialize(archive); }
	bool BinaryOutputArchive::Value(T& value)
	{
		if (depth >= SerializationLimits::MAX_DEPTH)
		{
			return false;
		}
		const usize offset = bytes.size();
		if (!Value(u64{ 0 }))
		{
			return false;
		}
		BinaryOutputArchive object(bytes, depth + 1);
		return value.Serialize(object) && PatchSize(offset, bytes.size() - offset - sizeof(u64));
	}

	template <class T>
	bool BinaryOutputArchive::Field(u32 id, T& value)
	{
		if (id == 0 || fieldIDs.size() >= SerializationLimits::MAX_FIELDS
			|| !fieldIDs.insert(id).second || !Value(id))
		{
			return false;
		}
		const usize offset = bytes.size();
		return Value(u64{ 0 }) && Value(value) && PatchSize(offset, bytes.size() - offset - sizeof(u64));
	}

	template <class T>
	bool BinaryOutputArchive::OptionalField(u32 id, T& value)
	{
		return Field(id, value);
	}

	inline bool BinaryOutputArchive::PatchSize(usize offset, u64 size)
	{
		if (offset > bytes.size() || bytes.size() - offset < sizeof(u64))
		{
			return false;
		}
		auto encoded = std::bit_cast<std::array<byte, sizeof(u64)>>(size);
		if constexpr (std::endian::native == std::endian::big)
		{
			std::ranges::reverse(encoded);
		}
		std::copy(encoded.begin(), encoded.end(), bytes.begin() + offset);
		return true;
	}

	inline BinaryInputArchive::BinaryInputArchive(
		std::span<const byte> bytes, usize& allocationBudget, u32 depth) noexcept
		: bytes(bytes), allocationBudget(allocationBudget), depth(depth)
	{
	}

	template <BinaryNumber T>
	bool BinaryInputArchive::Value(T& value)
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
			static_assert(std::numeric_limits<T>::is_iec559);
			return std::isfinite(value);
		}
		return true;
	}

	inline bool BinaryInputArchive::Value(std::string& value)
	{
		u32 length = 0;
		if (!Value(length) || length > bytes.size() - position || !Allocate(length, 1))
		{
			return false;
		}
		value.assign(reinterpret_cast<const char*>(bytes.data() + position), length);
		position += length;
		return Serializer::IsValidText(value);
	}

	template <class T, usize N>
	bool BinaryInputArchive::Value(std::array<T, N>& values)
	{
		for (auto& value : values)
		{
			if (!Value(value))
			{
				return false;
			}
		}
		return true;
	}

	template <class T>
	bool BinaryInputArchive::Value(std::vector<T>& values)
	{
		u32 count = 0;
		// Every supported element occupies at least one byte on disk.
		if (!Value(count) || count > bytes.size() - position || !Allocate(count, sizeof(T)))
		{
			return false;
		}
		values.resize(count);
		for (auto& value : values)
		{
			if (!Value(value))
			{
				return false;
			}
		}
		return true;
	}

	template <class T> requires requires(T& value, BinaryInputArchive& archive) { value.Serialize(archive); }
	bool BinaryInputArchive::Value(T& value)
	{
		u64 size = 0;
		if (depth >= SerializationLimits::MAX_DEPTH || !Value(size) || size > bytes.size() - position)
		{
			return false;
		}
		BinaryInputArchive object(bytes.subspan(position, static_cast<usize>(size)), allocationBudget, depth + 1);
		position += static_cast<usize>(size);
		return object.IndexFields() && value.Serialize(object);
	}

	template <class T>
	bool BinaryInputArchive::Field(u32 id, T& value)
	{
		const auto field = fields.find(id);
		if (field == fields.end())
		{
			return false;
		}
		BinaryInputArchive input(field->second, allocationBudget, depth);
		return input.Value(value) && input.AtEnd();
	}

	template <class T>
	bool BinaryInputArchive::OptionalField(u32 id, T& value)
	{
		return !fields.contains(id) || Field(id, value);
	}

	inline bool BinaryInputArchive::AtEnd() const noexcept
	{
		return position == bytes.size();
	}

	inline bool BinaryInputArchive::IndexFields()
	{
		while (!AtEnd())
		{
			u32 id = 0;
			u64 size = 0;
			if (fields.size() >= SerializationLimits::MAX_FIELDS || !Value(id) || id == 0
				|| !Value(size) || size > bytes.size() - position)
			{
				return false;
			}
			if (!fields.emplace(id, bytes.subspan(position, static_cast<usize>(size))).second)
			{
				return false;
			}
			position += static_cast<usize>(size);
		}
		return true;
	}

	inline bool BinaryInputArchive::Allocate(usize count, usize elementSize) noexcept
	{
		if (count > SerializationLimits::MAX_ELEMENTS || count > allocationBudget / elementSize)
		{
			return false;
		}
		allocationBudget -= count * elementSize;
		return true;
	}

	template <class T>
	std::expected<T, std::string> Serializer::Load(const std::filesystem::path& path, u32 resourceType)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file)
		{
			return std::unexpected("Cannot open binary resource: " + path.generic_string());
		}
		const auto size = file.tellg();
		if (size < 20 || size > static_cast<std::streamoff>(SerializationLimits::MAX_BYTES))
		{
			return std::unexpected("Invalid binary resource size: " + path.generic_string());
		}
		std::vector<byte> bytes(static_cast<usize>(size));
		file.seekg(0);
		if (!file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size())))
		{
			return std::unexpected("Cannot read binary resource: " + path.generic_string());
		}
		usize budget = SerializationLimits::MAX_BYTES;
		BinaryInputArchive input(bytes, budget);
		u32 magic = 0, type = 0, version = 0;
		u64 payloadSize = 0;
		T value{};
		if (!input.Value(magic) || magic != MAGIC || !input.Value(type) || type != resourceType
			|| !input.Value(version) || version != FORMAT_VERSION || !input.Value(payloadSize)
			|| payloadSize != bytes.size() - 20 || !input.Value(value) || !input.AtEnd())
		{
			return std::unexpected("Invalid or unsupported binary resource: " + path.generic_string());
		}
		return value;
	}

	template <class T>
	std::expected<void, std::string> Serializer::Save(
		const std::filesystem::path& path, u32 resourceType, T& value)
	{
		std::vector<byte> bytes;
		BinaryOutputArchive output(bytes);
		if (!output.Value(MAGIC) || !output.Value(resourceType) || !output.Value(FORMAT_VERSION)
			|| !output.Value(u64{ 0 }) || !output.Value(value))
		{
			return std::unexpected("Cannot serialize binary resource: " + path.generic_string());
		}
		output.PatchSize(12, bytes.size() - 20);
		std::ofstream file(path, std::ios::binary | std::ios::trunc);
		file.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
		file.flush();
		file.close();
		if (!file)
		{
			return std::unexpected("Cannot write binary resource: " + path.generic_string());
		}
		return {};
	}
}
