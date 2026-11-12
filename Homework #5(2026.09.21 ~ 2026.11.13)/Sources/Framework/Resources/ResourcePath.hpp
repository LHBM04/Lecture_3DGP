#pragma once

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
		return std::unexpected("리소스 분류 디렉터리를 찾을 수 없습니다: " + resourcePath.generic_string());
	}

	inline std::expected<std::filesystem::path, std::string> ResolveResourcePath(
		const std::filesystem::path& root, const std::string& relativePath)
	{
		if (relativePath.empty() || relativePath.find('\0') != std::string::npos)
		{
			return std::unexpected("리소스 참조 경로가 비어 있거나 유효하지 않습니다.");
		}
		auto path = std::filesystem::path(std::u8string(relativePath.begin(), relativePath.end())).lexically_normal();
		if (root.filename() == "Resources" && !path.empty() && *path.begin() == "Resources")
		{
			path = path.lexically_relative("Resources");
		}
		if (path.empty() || path.has_root_path() || *path.begin() == "..")
		{
			return std::unexpected("리소스 참조 경로는 루트 디렉터리 내부여야 합니다: " + relativePath);
		}
		return (root / path).lexically_normal();
	}
}
