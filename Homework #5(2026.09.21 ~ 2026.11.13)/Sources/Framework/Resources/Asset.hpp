#pragma once

#include <string>
#include <string_view>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class Asset
	{
	public:
		virtual ~Asset() noexcept = default;

		Asset(const Asset&) = delete;
		Asset& operator=(const Asset&) = delete;

		Asset(Asset&&) noexcept = default;
		Asset& operator=(Asset&&) noexcept = default;

		[[nodiscard]] virtual std::string Serialize() const = 0;
		[[nodiscard]] virtual bool Deserialize(std::string_view text) = 0;

	protected:
		Asset() noexcept = default;
	};
}
