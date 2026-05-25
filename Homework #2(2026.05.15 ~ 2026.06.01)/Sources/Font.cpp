#include "Precompiled.h"
#include "Font.h"

bool Font::Load(const std::filesystem::path& path_)
{
	SetPath(path_);

	if (!path_.empty())
	{
		const std::wstring candidate{ path_.stem().wstring() };
		if (!candidate.empty())
		{
			familyName = candidate;
		}
	}

	SetLoaded(true);
	return true;
}

void Font::Unload()
{
	SetLoaded(false);
}

const std::wstring& Font::GetFamilyName() const noexcept
{
	return familyName;
}

void Font::SetFamilyName(const std::wstring& familyName_) noexcept
{
	if (!familyName_.empty())
	{
		familyName = familyName_;
	}
}

int Font::GetSize() const noexcept
{
	return size;
}

void Font::SetSize(int size_) noexcept
{
	size = std::max(8, size_);
}

int Font::GetWeight() const noexcept
{
	return weight;
}

void Font::SetWeight(int weight_) noexcept
{
	weight = std::clamp(weight_, static_cast<int>(FW_THIN), static_cast<int>(FW_HEAVY));
}
