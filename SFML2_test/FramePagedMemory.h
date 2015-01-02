#pragma once

//memory created for one page and thrashed / released at the end of the frame

#include <list>

class FramePagedMemory 
{
public:

	//const int init_page_size = 5*1024*1024; // 5 MB
	//const int new_page_size = 2*1024*1024; // 2 MB
	FramePagedMemory(void);
	~FramePagedMemory(void);
};

