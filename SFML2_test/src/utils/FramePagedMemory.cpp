#include "utils/FramePagedMemory.h"

PagedVector pagedVectorStatic;

PagedVector::PagedVector():
	pageSize(5*1024*1024),
	current_byte_offset(0),
	current_page(0)
{
	//initialise one page:
	char* ptr = static_cast<char*>(malloc(pageSize));
	pages.push_back(std::make_pair(ptr, pageSize));
}

PagedVector::~PagedVector() {
	for(auto it = pages.begin() ; it != pages.end(); ++it) {
		free(it->first);
	}
}

void PagedVector::resetForNewFrame() {

	//remove pages except the first one
	auto it = pages.begin();
	++it;

	for( ; it != pages.end(); ) {
		free(it->first);
		pages.erase(it++);
	}

	//reset page count
	current_page = 0;
	current_byte_offset = 0;
}

