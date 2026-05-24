#pragma once

#include <filesystem>

class Resource
{
public:
	Resource() = default;
	virtual ~Resource() = default;

	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;

	Resource(Resource&&) = delete;
	Resource& operator=(Resource&&) = delete;

	[[nodiscard]] const std::filesystem::path& GetPath() const noexcept;
	void SetPath(const std::filesystem::path& path_);

	[[nodiscard]] bool IsLoaded() const noexcept;
	void SetLoaded(bool loaded_) noexcept;

private:
	std::filesystem::path path;
	bool loaded{ false };
};
