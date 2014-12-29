#pragma once

#include <functional>
#include <map>
#include <list>

typedef unsigned int time_tt ;

struct ActiveCallBackInfo;

struct CallBackInfo {
	bool isExecuting;
	std::function<void(time_tt,time_tt)> function;

	std::list<ActiveCallBackInfo>& targetCallbackList;
	std::list<ActiveCallBackInfo>::iterator targetCallbackPosition;

	void call(time_tt original_time, time_tt actual_time);

	explicit CallBackInfo(std::list<ActiveCallBackInfo>& targetCallbackList);
};

class CallBackSystem
{
	std::map<time_tt, std::list<CallBackInfo>> events;

public:
	CallBackSystem();

	void callAllUpToTime(time_tt time);

	ActiveCallBackInfo addCallback(time_tt time, 
		const std::function<void(time_tt,time_tt)>& function, 
		std::list<ActiveCallBackInfo>& targetCallbackList);

	~CallBackSystem();

private:
	CallBackSystem(CallBackSystem&);
	CallBackSystem& operator=(CallBackSystem&);
};


struct ActiveCallBackInfo {
	std::list<CallBackInfo>& list;
	std::list<CallBackInfo>::iterator callbackPosition;

	ActiveCallBackInfo(std::list<CallBackInfo>& list, std::list<CallBackInfo>::iterator& pos);
};

class CallBackReceiver
{
	std::list<ActiveCallBackInfo> activeCallbacks;

public:
	~CallBackReceiver();

	void createCallback(time_tt time, 
		CallBackSystem& cbs, 
		const std::function<void(time_tt,time_tt)>& function);

	void createPeriodicCallback(time_tt timeStart, time_tt period,
		CallBackSystem& cbs, 
		const std::function<void(time_tt,time_tt)>& function);

	void createPeriodicCallback(time_tt timeStart, time_tt period,
		time_tt last,
		CallBackSystem& cbs, 
		const std::function<void(time_tt,time_tt)>& function);

};