#pragma once

#include "Resource.h"

#include <string>

class Font final : public Resource
{
public:
	Font() = default;
	~Font() override = default;

	bool Load(const std::filesystem::path& path_) override;
	void Unload() override;

	[[nodiscard]] const std::wstring& GetFamilyName() const noexcept;
	void SetFamilyName(const std::wstring& familyName_) noexcept;

	[[nodiscard]] int GetSize() const noexcept;
	void SetSize(int size_) noexcept;

	[[nodiscard]] int GetWeight() const noexcept;
	void SetWeight(int weight_) noexcept;

private:
	std::wstring familyName{ L"Segoe UI" };
	int size{ 22 };
	int weight{ FW_NORMAL };
};
