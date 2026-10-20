#pragma once

namespace TUK::Framework
{
	namespace Memory
	{
		void Initialize();
		void Release();

		template <class T>
		constexpr void* Allocate(size_t size)
		{
			return ::operator new(size * sizeof(T));
		}


		template <class T>
		constexpr void Deallocate(void* ptr)
		{
			::operator delete(ptr);
		}
	}
}
