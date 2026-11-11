#pragma once

#include <array>
#include <filesystem>
#include <string>
#include <vector>

#include "../Core/Serializer.hpp"

namespace TUK::Framework
{
	// Field IDs are part of the on-disk contract. Never reuse an existing ID.
	enum class ResourceType : u32
	{
		MODEL = 1,
		MESH = 2,
		MATERIAL = 3,
		FONT = 4
	};

	struct RenderPartMetadata final
	{
		std::string mesh;
		std::string material;
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct ModelNodeMetadata final
	{
		std::string name;
		std::array<f32, 3> localPosition{ 0, 0, 0 };
		std::array<f32, 4> localRotation{ 0, 0, 0, 1 };
		std::array<f32, 3> localScale{ 1, 1, 1 };
		std::vector<RenderPartMetadata> renderParts;
		std::vector<ModelNodeMetadata> children;
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct ModelMetadata final
	{
		ModelNodeMetadata root;
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct MeshMetadata final
	{
		std::vector<std::array<f32, 3>> positions;
		std::vector<std::array<f32, 3>> normals;
		std::vector<std::array<f32, 2>> uv0;
		std::vector<u32> indices;
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct TextureBindingMetadata final
	{
		u32 slot = 0;
		std::string path;
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	struct MaterialMetadata final
	{
		std::array<f32, 4> baseColor{ 1, 1, 1, 1 };
		std::array<f32, 4> emissiveColor{ 0, 0, 0, 1 };
		std::array<f32, 4> specularColor{ 1, 1, 1, 1 };
		f32 metallic = 0;
		f32 roughness = 0.5f;
		u32 pass = 0;
		std::string vertexShader;
		std::string pixelShader;
		std::vector<TextureBindingMetadata> textures;
		template <class TArchive> bool Serialize(TArchive& archive);
	};

	// Resource path policy belongs here, not in individual loader utility functions.
	class ResourceMetadata final
	{
	public:
		[[nodiscard]] static std::expected<std::filesystem::path, std::string> FindRoot(
			const std::filesystem::path& resourcePath, const std::filesystem::path& category);
		[[nodiscard]] static std::expected<std::filesystem::path, std::string> ResolvePath(
			const std::filesystem::path& root, const std::string& relativePath);
	};

	template <class TArchive>
	bool RenderPartMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, mesh) && archive.Field(2, material);
	}

	template <class TArchive>
	bool ModelNodeMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, name) && archive.Field(2, localPosition)
			&& archive.Field(3, localRotation) && archive.Field(4, localScale)
			&& archive.Field(5, renderParts) && archive.Field(6, children);
	}

	template <class TArchive>
	bool ModelMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, root);
	}

	template <class TArchive>
	bool MeshMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, positions) && archive.Field(2, normals)
			&& archive.OptionalField(3, uv0) && archive.Field(4, indices);
	}

	template <class TArchive>
	bool TextureBindingMetadata::Serialize(TArchive& archive)
	{
		return archive.Field(1, slot) && archive.Field(2, path);
	}

	template <class TArchive>
	bool MaterialMetadata::Serialize(TArchive& archive)
	{
		return archive.OptionalField(1, baseColor) && archive.OptionalField(2, emissiveColor)
			&& archive.OptionalField(3, specularColor) && archive.OptionalField(4, metallic)
			&& archive.OptionalField(5, roughness) && archive.OptionalField(6, pass)
			&& archive.Field(7, vertexShader) && archive.Field(8, pixelShader)
			&& archive.OptionalField(9, textures);
	}

	inline std::expected<std::filesystem::path, std::string> ResourceMetadata::FindRoot(
		const std::filesystem::path& resourcePath, const std::filesystem::path& category)
	{
		for (auto directory = resourcePath.parent_path(); !directory.empty(); directory = directory.parent_path())
		{
			if (directory.filename() == category)
			{
				return directory.parent_path();
			}
			if (directory == directory.parent_path())
			{
				break;
			}
		}
		return std::unexpected("Resource category directory not found: " + resourcePath.generic_string());
	}

	inline std::expected<std::filesystem::path, std::string> ResourceMetadata::ResolvePath(
		const std::filesystem::path& root, const std::string& relativePath)
	{
		if (relativePath.empty() || relativePath.find('\0') != std::string::npos)
		{
			return std::unexpected("Empty or invalid resource reference");
		}
		const auto path = std::filesystem::path(std::u8string(relativePath.begin(), relativePath.end())).lexically_normal();
		if (path.empty() || path.has_root_path() || *path.begin() == "..")
		{
			return std::unexpected("Resource reference must stay inside its root: " + relativePath);
		}
		return (root / path).lexically_normal();
	}
}
