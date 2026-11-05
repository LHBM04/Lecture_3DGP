#pragma once

#include <memory_resource>

#include "Common.hpp"

namespace TUK::Framework
{
	inline constexpr usize DEFAULT_POOL_SIZE = 128ULL * 1024ULL * 1024ULL;

	namespace Memory
	{
		void Initialize(usize poolSize = DEFAULT_POOL_SIZE) noexcept;
		void Shutdown() noexcept;

		[[nodiscard]] std::pmr::memory_resource* GetResource() noexcept;
	}
}
