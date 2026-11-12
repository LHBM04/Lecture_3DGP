#pragma once

#include <expected>
#include <filesystem>
#include <string>

namespace TUK::Framework
{
	[[nodiscard]] inline std::expected<std::filesystem::path, std::string> FindResourceRoot(
		const std::filesystem::path& resourcePath, const std::filesystem::path& category);
	[[nodiscard]] inline std::expected<std::filesystem::path, std::string> ResolveResourcePath(
		const std::filesystem::path& root, const std::string& relativePath);

	inline std::expected<std::filesystem::path, std::string> FindResourceRoot(
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

	inline std::expected<std::filesystem::path, std::string> ResolveResourcePath(
		const std::filesystem::path& root, const std::string& relativePath)
	{
		if (relativePath.empty() || relativePath.find('\0') != std::string::npos)
		{
			return std::unexpected("Empty or invalid resource reference");
		}
		auto path = std::filesystem::path(std::u8string(relativePath.begin(), relativePath.end())).lexically_normal();
		if (root.filename() == "Resources" && !path.empty() && *path.begin() == "Resources")
		{
			path = path.lexically_relative("Resources");
		}
		if (path.empty() || path.has_root_path() || *path.begin() == "..")
		{
			return std::unexpected("Resource reference must stay inside its root: " + relativePath);
		}
		return (root / path).lexically_normal();
	}
}
