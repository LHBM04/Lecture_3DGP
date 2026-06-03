#pragma once

#include <filesystem>
#include <memory>

#include "Resource.h"

class IResourceLoader
{
public:
	virtual ~IResourceLoader() = default;
	[[nodiscard]] virtual std::unique_ptr<Resource> Load(const std::filesystem::path& path_) = 0;
};
