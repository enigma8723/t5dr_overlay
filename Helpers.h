#pragma once

#include "Memory.hpp"

#define BYTESWAP_INT64(x) _byteswap_uint64(x)
#define BYTESWAP_INT32(x) (((x & 0xFF000000) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x & 0xFF) << 24))
#define BYTESWAP_INT16(x) (((x & 0xFFFF) >> 8) | ((x & 0xFF) << 8))

#define DEREF_INT8(x) (*(int8_t*)(x))
#define DEREF_UINT8(x) (*(uint8_t*)(x))
#define DEREF_INT16(x) (*(int16_t*)(x))
#define DEREF_UINT16(x) (*(uint16_t*)(x))
#define DEREF_INT32(x) (*(int32_t*)(x))
#define DEREF_UINT32(x) (*(uint32_t*)(x))
#define DEREF_INT64(x) (*(int64_t*)(x))
#define DEREF_UINT64(x) (*(uint64_t*)(x))

// Helper class to iterate on struct pts
template<class T>
class StructIterator
{
public:
	struct Iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;

		Iterator(T* ptr) : m_ptr(ptr) {}

		reference operator[](int n) const { return m_ptr[n]; }
		reference operator*() const { return *m_ptr; }
		pointer operator->() { return m_ptr; }
		Iterator& operator++() { m_ptr++; return *this; }
		Iterator& operator+=(int n) { m_ptr += n; return *this; }
		Iterator operator++(int) { Iterator tmp = *this; (*this) += 1; return tmp; }
		Iterator operator+(int x) { return Iterator(&m_ptr[x]); }
		friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; }
		friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; }

	private:
		T* m_ptr;
	};

	Iterator begin() const { return Iterator(&m_ptr[0]); }
	Iterator end() const { return Iterator(&m_ptr[m_size]); }
	uint64_t size() const { return m_size; }
	void set_size(uint64_t new_size) { m_size = new_size; }
	T* operator[](size_t n) const { return &m_ptr[n]; }

	StructIterator() { m_ptr = nullptr, m_size = 0; }
	StructIterator(void* ptr, size_t size) { Set(ptr, size); }
	StructIterator(void* base_ptr, void* ptr, size_t size) { Set(base_ptr, ptr, size); }
	StructIterator(void* base_ptr, uint64_t ptr, size_t size) { Set(base_ptr, ptr, size); }
	StructIterator(uint64_t base_ptr, void* ptr, size_t size) { Set(base_ptr, ptr, size); }
	StructIterator(uint64_t base_ptr, uint64_t ptr, size_t size) { Set(base_ptr, ptr, size); }
	void Set(void* ptr, size_t size) { m_ptr = (T*)ptr, m_size = size; }
	void Set(void* base_ptr, void* ptr, size_t size) { m_ptr = (T*)((uint64_t)base_ptr + (uint64_t)ptr), m_size = size; }
	void Set(void* base_ptr, uint64_t ptr, size_t size) { m_ptr = (T*)((uint64_t)base_ptr + ptr), m_size = size; }
	void Set(uint64_t base_ptr, void* ptr, size_t size) { m_ptr = (T*)(base_ptr + (uint64_t)ptr), m_size = size; }
	void Set(uint64_t base_ptr, uint64_t ptr, size_t size) { m_ptr = (T*)(base_ptr + ptr), m_size = size; }
private:
	T* m_ptr = nullptr;
	size_t m_size = 0;
};

namespace ByteswapHelpers
{
	void SWAP_INT16(void* x);
	void SWAP_INT32(void* x);
	void SWAP_INT64(void* x);
};

namespace ProcessHandling {
	HANDLE AttachToProcess(std::string processName, Memory* memory);
}