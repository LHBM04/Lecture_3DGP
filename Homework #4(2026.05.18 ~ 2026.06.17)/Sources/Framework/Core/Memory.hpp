#pragma once

#include <cstddef>
#include <memory_resource>
#include <utility>

namespace TUK::Framework
{
	using MemoryResource = std::pmr::memory_resource*;

	namespace Memory
	{
		/** 메모리 풀 할당 */
		void Initialize(std::size_t persistentBufferSize, std::size_t frameBufferSize);

		/** 메모리 풀 해제 */
		void Release();

		/** 영구 보관용 메모리 리소스 반환 */
		[[nodiscard]] MemoryResource GetPersistentResource() noexcept;

		/** 영구 풀에서 단일 객체 생성 */
		template <class T, class... TArgs>
		[[nodiscard]] T* Allocate(TArgs&&... args)
		{
			std::pmr::polymorphic_allocator<T> alloc(GetPersistentResource());
			return alloc.new_object<T>(std::forward<TArgs>(args)...);
		}

		/** 영구 풀에서 단일 객체 해제 */
		template <class T>
		void Deallocate(T* ptr)
		{
			if (ptr)
			{
				std::pmr::polymorphic_allocator<T> alloc(GetPersistentResource());
				alloc.delete_object(ptr);
			}
		}
	}
}
