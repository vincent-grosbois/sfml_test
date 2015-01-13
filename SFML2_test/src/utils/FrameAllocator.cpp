#include "utils/FrameAllocator.h"
#include <iostream>

MemoryPages memoryPagesStatic;

MemoryPages::MemoryPages():
	pageSize(5*1024*1024), // 5 MB 
	current_byte_offset(0),
	current_page(0)
{
	//initialise one page:
	char* ptr = static_cast<char*>(malloc(pageSize));
	pages.push_back(std::make_pair(ptr, pageSize));
}

MemoryPages::~MemoryPages() {
	for(auto it = pages.begin() ; it != pages.end(); ++it) {
		free(it->first);
	}
}

void MemoryPages::resetForNewFrame() {

	//remove pages except the first one
	auto it = pages.begin();
	++it;

	for( ; it != pages.end(); ) {
		free(it->first);
		it = pages.erase(it);
	}

	//reset page count
	current_page = 0;
	current_byte_offset = 0;
}