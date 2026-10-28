#pragma once

#include <fstream>
#include <string>
#include <string_view>

class Resource
{
public:
	Resource() = default;
	virtual ~Resource() = default;

	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;

	virtual bool Load() = 0;
	virtual void Unload() = 0;

	[[nodiscard]] const std::wstring& GetPath() const noexcept;
	void SetPath(std::wstring_view path_);

	[[nodiscard]] const std::wstring& GetName() const noexcept;
	void SetName(std::wstring_view name_);

protected:
	[[nodiscard]] bool HasTag(std::ifstream& file_, std::string_view expectedTag_) const;
	[[nodiscard]] std::string ReadTag(std::ifstream& file_) const;
	[[nodiscard]] std::wstring ReadString(std::ifstream& file_) const;
	[[nodiscard]] std::wstring ReadRemainingString(std::ifstream& file_) const;

	template <class TValue>
	bool ReadValue(std::ifstream& file_, TValue& value_) const
	{
		return static_cast<bool>(file_.read(reinterpret_cast<char*>(&value_), sizeof(TValue)));
	}

	std::wstring path;
	std::wstring name;
};
