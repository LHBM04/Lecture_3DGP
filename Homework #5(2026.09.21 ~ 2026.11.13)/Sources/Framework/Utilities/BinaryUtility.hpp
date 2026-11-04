#pragma once

#include <array>
#include <bit>
#include <concepts>
#include <cstring>
#include <istream>
#include <limits>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	template <class TValue>
	concept BinaryScalar = (std::integral<TValue> && !std::same_as<TValue, bool>) || std::floating_point<TValue>;

	template <class TObject>
	concept Serializable = requires(const TObject& object)
	{
		{ object.Serialize() } -> std::same_as<std::string>;
	};

	template <class TObject>
	concept Deserializable = requires(TObject& object, std::string_view text)
	{
		{ object.Deserialize(text) } -> std::same_as<bool>;
	};

	namespace BinaryUtility
	{
		template <BinaryScalar TValue>
		[[nodiscard]] std::string ToBin(const TValue& value)
		{
			return std::string(reinterpret_cast<const char*>(&value), sizeof(TValue));
		}

		template <BinaryScalar TValue>
		[[nodiscard]] TValue FromBin(std::string_view text) noexcept
		{
			if (text.size() != sizeof(TValue))
			{
				return {};
			}

			std::array<byte, sizeof(TValue)> bytes{};
			std::memcpy(bytes.data(), text.data(), sizeof(TValue));
			return std::bit_cast<TValue>(bytes);
		}

		template <Serializable TObject>
		[[nodiscard]] std::string ToBin(const TObject& object)
		{
			return object.Serialize();
		}

		template <Deserializable TObject>
		[[nodiscard]] TObject FromBin(std::string_view text)
		{
			TObject object{};
			if (object.Deserialize(text))
			{
				return object;
			}

			return {};
		}

		template <BinaryScalar TValue>
		bool ToBin(std::ostream& stream, const TValue& value)
		{
			stream.write(reinterpret_cast<const char*>(&value), sizeof(TValue));
			return static_cast<bool>(stream);
		}

		inline bool ToBin(std::ostream& stream, std::string_view text)
		{
			if (text.size() > (std::numeric_limits<u8>::max)())
			{
				stream.setstate(std::ios::failbit);
				return false;
			}

			if (!ToBin(stream, static_cast<u8>(text.size())))
			{
				return false;
			}

			if (!text.empty())
			{
				stream.write(text.data(), static_cast<std::streamsize>(text.size()));
			}

			return static_cast<bool>(stream);
		}

		template <Serializable TObject>
		bool ToBin(std::ostream& stream, const TObject& object)
		{
			const std::string bytes = object.Serialize();
			if (bytes.empty())
			{
				stream.setstate(std::ios::failbit);
				return false;
			}

			stream.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
			return static_cast<bool>(stream);
		}

		inline bool FromBin(std::istream& stream, std::span<byte> destination)
		{
			if (destination.size() > static_cast<usize>((std::numeric_limits<std::streamsize>::max)()))
			{
				stream.setstate(std::ios::failbit);
				return false;
			}

			if (!destination.empty())
			{
				stream.read(reinterpret_cast<char*>(destination.data()), static_cast<std::streamsize>(destination.size()));
			}

			return static_cast<bool>(stream);
		}

		template <BinaryScalar TValue>
		bool FromBin(std::istream& stream, TValue& value)
		{
			std::array<byte, sizeof(TValue)> bytes{};
			if (!FromBin(stream, bytes))
			{
				value = TValue{};
				return false;
			}

			value = std::bit_cast<TValue>(bytes);
			return true;
		}

		template <BinaryScalar TValue>
		[[nodiscard]] TValue FromBin(std::istream& stream)
		{
			TValue value{};
			FromBin(stream, value);
			return value;
		}

		inline bool FromBin(std::istream& stream, std::string& text)
		{
			u8 length = 0;
			if (!FromBin(stream, length))
			{
				text.clear();
				return false;
			}

			text.assign(length, '\0');
			return FromBin(stream, std::as_writable_bytes(std::span<char>(text)));
		}

		template <class TString>
			requires std::same_as<TString, std::string>
		[[nodiscard]] TString FromBin(std::istream& stream)
		{
			std::string text;
			FromBin(stream, text);
			return text;
		}

		template <Deserializable TObject>
			requires std::is_standard_layout_v<TObject>
		bool FromBin(std::istream& stream, TObject& object)
		{
			std::array<char, sizeof(TObject)> buffer{};
			if (!FromBin(stream, std::as_writable_bytes(std::span<char>(buffer))))
			{
				object = TObject{};
				return false;
			}

			if (!object.Deserialize(std::string_view(buffer.data(), buffer.size())))
			{
				stream.setstate(std::ios::failbit);
				object = TObject{};
				return false;
			}

			return true;
		}

		template <Deserializable TObject>
			requires std::is_standard_layout_v<TObject>
		[[nodiscard]] TObject FromBin(std::istream& stream)
		{
			TObject object{};
			FromBin(stream, object);
			return object;
		}
	}
}
