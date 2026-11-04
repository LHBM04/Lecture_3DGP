#include "Precompiled.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	std::string ShaderAsset::Serialize() const
	{
		return {};
	}

	bool ShaderAsset::Deserialize(std::string_view)
	{
		return false;
	}
}
