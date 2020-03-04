#pragma once

#include <cstdlib>
#include <new>
#include <limits>

template <class T>
struct tracked_allocator
{
	typedef T value_type;
	size_t& total_bytes;

	tracked_allocator(size_t& total_bytes)
		: total_bytes{ total_bytes }
	{
	}

	template <class U> constexpr tracked_allocator(const tracked_allocator<U>& other) noexcept
		: total_bytes{ other.total_bytes }
	{
	}

	[[nodiscard]] T* allocate(std::size_t n)
	{
		if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
			throw std::bad_alloc();

		if (auto p = static_cast<T*>(std::malloc(n * sizeof(T))))
		{
			total_bytes += n * sizeof(T);
			return p;
		}

		throw std::bad_alloc();
	}

	void deallocate(T* p, std::size_t n) noexcept
	{
		total_bytes -= n * sizeof(T);
		std::free(p);
	}
};

template <class T, class U>
bool operator==(const tracked_allocator <T>&, const tracked_allocator <U>&) { return false; }

template <class T, class U>
bool operator!=(const tracked_allocator <T>&, const tracked_allocator <U>&) { return true; }

