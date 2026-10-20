#pragma once

#include <cstddef>
#include <memory_resource>
#include <utility>

namespace TUK::Framework
{
	namespace Memory
	{
		using MemoryResource = std::pmr::memory_resource*;

		/** 엔진 시작 시 호출하여 메모리 풀 사전 할당 */
		void Initialize(std::size_t persistentBufferSize, std::size_t frameBufferSize);

		/** 엔진 종료 시 호출하여 모든 메모리 해제 */
		void Release();

		/** 매 프레임 종료 시 호출하여 프레임 임시 메모리 일괄 되감기 */
		void EndFrame();

		/** 영구 보관용 메모리 리소스 반환 */
		[[nodiscard]] MemoryResource GetPersistentResource() noexcept;

		/** 1프레임 임시용 메모리 리소스 반환 */
		[[nodiscard]] MemoryResource GetFrameResource() noexcept;

		/** 영구 풀에서 단일 객체 생성 */
		template <class T, class... TArgs>
		[[nodiscard]] T* Allocate(TArgs&&... args)
		{
			std::pmr::polymorphic_allocator<T> alloc(GetPersistentResource());
			return alloc.new_object<T>(std::forward<TArgs>(args)...);
		}

		/** 영구 풀에서 배열 할당 */
		template <class T>
		[[nodiscard]] T* AllocateArray(std::size_t count)
		{
			std::pmr::polymorphic_allocator<T> alloc(GetPersistentResource());
			return alloc.allocate_object<T>(count);
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

		/** 영구 풀에서 배열 해제 */
		template <class T>
		void DeallocateArray(T* ptr, std::size_t count)
		{
			if (ptr)
			{
				std::pmr::polymorphic_allocator<T> alloc(GetPersistentResource());
				alloc.deallocate_object(ptr, count);
			}
		}

		/** 프레임 임시 풀에서 단일 객체 생성 */
		template <class T, class... TArgs>
		[[nodiscard]] T* AllocateFrame(TArgs&&... args)
		{
			std::pmr::polymorphic_allocator<T> alloc(GetFrameResource());
			return alloc.new_object<T>(std::forward<TArgs>(args)...);
		}

		/** 프레임 임시 풀에서 배열 할당 */
		template <class T>
		[[nodiscard]] T* AllocateFrameArray(std::size_t count)
		{
			std::pmr::polymorphic_allocator<T> alloc(GetFrameResource());
			return alloc.allocate_object<T>(count);
		}
	}
}
