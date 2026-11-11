#pragma once

#include <expected>
#include <filesystem>
#include <string>

#include "ResourceMetadata.hpp"

namespace TUK::Framework
{
	class ResourceImporter final
	{
	public:
		template <class T>
		[[nodiscard]] static std::expected<T, std::string> Load(const std::filesystem::path& path, ResourceType type);
	};

	template <class T>
	std::expected<T, std::string> ResourceImporter::Load(const std::filesystem::path& path, ResourceType type)
	{
		auto reader = BinaryReader::Open(path);
		if (!reader)
		{
			return std::unexpected(reader.error());
		}

		if (reader->IsLegacy())
		{
			if constexpr (requires(T& value, BinaryInputArchive& archive) { value.Serialize(archive); })
			{
				return Serializer::Load<T>(path, static_cast<u32>(type));
			}
			return std::unexpected("This resource must be re-exported with TUK.Editor: " + path.generic_string());
		}

		T value{};
		if (!reader->Read(value) || !reader->IsAtEnd())
		{
			return std::unexpected("Invalid sequential export data: " + path.generic_string());
		}
		return value;
	}
}
