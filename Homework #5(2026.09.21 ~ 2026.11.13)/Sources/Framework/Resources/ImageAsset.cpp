#include "Precompiled.hpp"
#include "ImageAsset.hpp"

namespace TUK::Framework
{
	std::string ImageAsset::Serialize() const
	{
		return {};
	}

	bool ImageAsset::Deserialize(std::string_view)
	{
		return false;
	}
}
