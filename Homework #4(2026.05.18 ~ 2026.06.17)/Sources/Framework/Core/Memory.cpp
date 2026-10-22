#include "Precompiled.hpp"
#include "Memory.hpp"

#include "Logger.hpp"

namespace
{
	// 영구 메모리 풀
	void* persistentBuffer = nullptr;
	std::pmr::monotonic_buffer_resource* persistentArena = nullptr;
	std::pmr::synchronized_pool_resource* persistentPool = nullptr;
}

namespace TUK::Framework
{
	namespace Memory
	{
		void Initialize(std::size_t persistentBufferSize, std::size_t frameBufferSize)
		{
#ifdef _DEBUG
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

			// 영구 메모리 확보
			persistentBuffer = std::malloc(persistentBufferSize);
			persistentArena = new std::pmr::monotonic_buffer_resource(
				persistentBuffer, persistentBufferSize, std::pmr::null_memory_resource()
			);
			persistentPool = new std::pmr::synchronized_pool_resource(persistentArena);
		}

		void Release()
		{
			// 영구 메모리 해제
			if (persistentPool)
			{
				delete persistentPool;
				persistentPool = nullptr;
			}
			if (persistentArena)
			{
				delete persistentArena;
				persistentArena = nullptr;
			}
			if (persistentBuffer)
			{
				std::free(persistentBuffer);
				persistentBuffer = nullptr;
			}
		}

		MemoryResource GetPersistentResource() noexcept
		{
			return persistentPool;
		}
	}
}
