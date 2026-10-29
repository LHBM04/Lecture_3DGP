#pragma once

namespace TUK::Framework
{
	class Asset
	{
	public:
		~Asset() noexcept = default;

		Asset(const Asset&) = delete;
		Asset& operator=(const Asset&) = delete;

		Asset(Asset&&) noexcept = default;
		Asset& operator=(Asset&&) noexcept = default;

	protected:
		Asset() noexcept = default;
	};
}
