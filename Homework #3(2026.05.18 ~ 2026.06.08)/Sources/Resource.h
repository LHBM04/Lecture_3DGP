#pragma once

#include <filesystem>
#include <string>
#include <string_view>

class Resource
{
public:
	Resource(std::wstring_view resourceName_ = {}, std::filesystem::path sourcePath_ = {});
	virtual ~Resource() = default;

	virtual bool Load() = 0;
	virtual void Unload() = 0;

	[[nodiscard]] const std::wstring& GetName() const noexcept;
	void SetName(std::wstring_view resourceName_);

	[[nodiscard]] const std::filesystem::path& GetSourcePath() const noexcept;
	void SetSourcePath(std::filesystem::path sourcePath_);
	[[nodiscard]] const std::filesystem::path& GetPath() const noexcept;
	void SetPath(std::filesystem::path sourcePath_);

	[[nodiscard]] bool IsLoaded() const noexcept;

protected:
	void MarkLoaded(bool isLoaded_) noexcept;

private:
	std::wstring resourceName;
	std::filesystem::path sourcePath;
	bool isLoaded{ false };
};
