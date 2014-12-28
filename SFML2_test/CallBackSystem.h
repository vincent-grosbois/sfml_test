#pragma once

#include <functional>
#include <map>
#include <list>

class CallBackSystem
{


	std::map<unsigned int, std::list<std::function<int(void)>>> a;

public:
	CallBackSystem(void);
	~CallBackSystem(void);
};

