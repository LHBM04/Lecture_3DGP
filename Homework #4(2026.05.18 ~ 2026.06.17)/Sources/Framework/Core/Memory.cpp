#include "Precompiled.hpp"
#include "Memory.hpp"

#include "Logger.hpp"

namespace
{
	// 영구 메모리 풀
	void* persistentBuffer = nullptr;
	std::pmr::monotonic_buffer_resource* persistentArena = nullptr;
	std::pmr::synchronized_pool_resource* persistentPool = nullptr;

	// 프레임 임시 메모리 풀
	void* frameBuffer = nullptr;
	std::pmr::monotonic_buffer_resource* frameArena = nullptr;
}

namespace TUK::Framework
{
	namespace Memory
	{
		void Initialize(std::size_t persistentBufferSize, std::size_t frameBufferSize)
		{
			// 영구 메모리 확보
			persistentBuffer = std::malloc(persistentBufferSize);
			persistentArena = new std::pmr::monotonic_buffer_resource(
				persistentBuffer, persistentBufferSize, std::pmr::null_memory_resource()
			);
			persistentPool = new std::pmr::synchronized_pool_resource(persistentArena);

			// 프레임 임시 메모리 초기화
			frameBuffer = std::malloc(frameBufferSize);
			frameArena = new std::pmr::monotonic_buffer_resource(
				frameBuffer, frameBufferSize, std::pmr::null_memory_resource()
			);

			Logger::Info("메모리 풀 초기화! persistentBufferSize={} bytes, frameBufferSize={} bytes", persistentBufferSize, frameBufferSize);
		}

		void Release()
		{
			// 프레임 메모리 해제
			if (frameArena)
			{
				delete frameArena;
				frameArena = nullptr;
			}
			if (frameBuffer)
			{
				std::free(frameBuffer);
				frameBuffer = nullptr;
			}

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

		void EndFrame()
		{
			if (frameArena)
			{
				frameArena->release();
			}
		}

		MemoryResource GetPersistentResource() noexcept
		{
			return persistentPool;
		}

		MemoryResource GetFrameResource() noexcept
		{
			return frameArena;
		}
	}
}
