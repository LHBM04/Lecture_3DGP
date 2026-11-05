#include "Precompiled.hpp"
#include "Allocator.hpp"

#include <cstddef>
#include <memory_resource>
#include <optional>

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	namespace
	{
		constexpr usize BUFFER_ALIGNMENT = alignof(std::max_align_t);

		void* buffer = nullptr;
		std::optional<std::pmr::monotonic_buffer_resource> arenaResource;
		std::optional<std::pmr::unsynchronized_pool_resource> poolResource;
		std::pmr::memory_resource* previousResource = nullptr;
	}

	namespace Memory
	{
		void Initialize(usize poolSize) noexcept
		{
			ASSERT(!buffer);
			ASSERT(poolSize > 0);

			buffer = _aligned_malloc(poolSize, BUFFER_ALIGNMENT);
			if (!buffer)
			{
				Logger::LogError("Failed to allocate memory pool ({} bytes).", poolSize);
				ABORT();
			}

			arenaResource.emplace(buffer, poolSize, std::pmr::null_memory_resource());
			poolResource.emplace(&*arenaResource);

			previousResource = std::pmr::set_default_resource(&*poolResource);

			Logger::LogInfo("Memory pool initialized ({} MB).", poolSize / (1024ULL * 1024ULL));
		}

		void Shutdown() noexcept
		{
			if (!buffer)
			{
				return;
			}

			std::pmr::set_default_resource(previousResource);
			previousResource = nullptr;

			poolResource.reset();
			arenaResource.reset();

			_aligned_free(buffer);
			buffer = nullptr;

			Logger::LogInfo("Memory pool shutdown.");
		}

		std::pmr::memory_resource* GetResource() noexcept
		{
			ASSERT(poolResource.has_value());
			return &*poolResource;
		}
	}
}
