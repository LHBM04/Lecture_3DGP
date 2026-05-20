#pragma once

class Asset
{
public:
	virtual ~Asset() noexcept = default;

	Asset(const Asset&) = delete;
	Asset& operator=(const Asset&) = delete;

	Asset(Asset&&) = delete;
	Asset& operator=(Asset&&) = delete;

	[[nodiscard]] const std::filesystem::path& GetPath() const noexcept;
	[[nodiscard]] bool IsLoaded() const noexcept;

	virtual bool Load(const std::filesystem::path& path_) = 0;
	virtual void Unload() noexcept = 0;

protected:
	Asset() noexcept = default;

	void SetPath(const std::filesystem::path& path_);
	void ClearPath() noexcept;
	void SetLoaded(bool isLoaded_) noexcept;

private:
	std::filesystem::path path;
	bool isLoaded{ false };
};
