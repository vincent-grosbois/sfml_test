#pragma once

//memory created for one page and thrashed / released at the end of the frame
#include <vector>

class PagedVector {

public :
	PagedVector();
	~PagedVector();

	void resetForNewFrame();

	template<class T>
	T* getNextAdress(int n) {

		size_t align = std::alignment_of<T>::value;
		size_t size = n * sizeof(T);

		//get next available adress for type with given size and aligment

		char* current_page_adress = pages[current_page].first + current_byte_offset;
		char* end_page_adress = pages[current_page].first + pages.back().second;

		size_t remainder_align = reinterpret_cast<uintptr_t>(current_page_adress) % align;
		size_t extra_alignment_padding = remainder_align==0 ? 0 : (align - remainder_align);

		char* next_aligned_adress = current_page_adress + extra_alignment_padding;
		char* end_aligned_adress = next_aligned_adress + size;

		if(end_aligned_adress < end_page_adress) {
			current_byte_offset += size + extra_alignment_padding;
			return reinterpret_cast<T*>(next_aligned_adress);
		}

		//no more space on the current page, have to reallocate a new one:

		//in the rare case where the object size > page size
		size_t nextPageSize = std::max(pageSize, size);

		//malloc is always aligned : 
		char* ptr = static_cast<char*>(malloc(nextPageSize));
		pages.push_back(std::make_pair(ptr, nextPageSize));
		current_byte_offset = size;
		++current_page;
		return reinterpret_cast<T*>(ptr);
	}

private :
	std::vector< std::pair<char* , size_t > > pages;

	size_t pageSize; // <* in bytes
	size_t current_byte_offset;
	int current_page;

	//non-copiable:
	PagedVector(PagedVector&);
	PagedVector& operator=(PagedVector&);
};

extern PagedVector pagedVectorStatic;

template<typename T>
class FrameAllocator {
public : 
	//    typedefs
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

public : 
	//    convert an allocator<T> to allocator<U>
	template<typename U>
	struct rebind {
		typedef FrameAllocator<U> other;
	};

public : 
	FrameAllocator() {
		pagedVector = &pagedVectorStatic;
	}

	~FrameAllocator() {}

	explicit FrameAllocator(const FrameAllocator& other) {
		pagedVector = other.pagedVector;
	}

	template<typename U>
	explicit FrameAllocator(const FrameAllocator<U>& other) {
		pagedVector = other.pagedVector;
	}


	//    address
	pointer address(reference r) { return &r; }
	const_pointer address(const_reference r) { return &r; }

	//    memory allocation
	pointer allocate(size_type cnt, 
		typename std::allocator<void>::const_pointer = 0) { 
			return reinterpret_cast<pointer>(pagedVector->getNextAdress<T>(cnt)); 
	}

	void deallocate(pointer p, size_type) { 
		//do nothing, deallocation is handled through pagedVector member
	}

	//    size
	size_type max_size() const { 
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	//    construction/destruction
	void construct(pointer p, const T& t) { new(p) T(t); }
	void destroy(pointer p) { p->~T(); }

	bool operator==(FrameAllocator const&) { return true; }
	bool operator!=(FrameAllocator const&) { return false; }


	PagedVector* pagedVector;
};

