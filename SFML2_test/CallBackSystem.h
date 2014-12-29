#pragma once

#include <functional>
#include <map>
#include <list>

typedef int frame_time_t;

struct ActiveCallBackInfo;

struct CallBackInfo {
	bool isExecuting;
	std::function<void(frame_time_t,frame_time_t)> function;

	std::list<ActiveCallBackInfo>& targetCallbackList;
	std::list<ActiveCallBackInfo>::iterator targetCallbackPosition;

	void call(frame_time_t original_time, frame_time_t actual_time);

	explicit CallBackInfo(std::list<ActiveCallBackInfo>& targetCallbackList);
};

class CallBackSystem
{
	std::map<frame_time_t, std::list<CallBackInfo>> events;

public:
	CallBackSystem();

	void callAllUpToTime(frame_time_t time);

	ActiveCallBackInfo addCallback(frame_time_t time, 
		const std::function<void(frame_time_t,frame_time_t)>& function, 
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
	virtual ~CallBackReceiver();

	void createCallback(frame_time_t time, 
		CallBackSystem& cbs, 
		const std::function<void(frame_time_t,frame_time_t)>& function);

	void createPeriodicCallback(frame_time_t timeStart, frame_time_t period,
		CallBackSystem& cbs, 
		const std::function<void(frame_time_t,frame_time_t)>& function);

	void createPeriodicCallback(frame_time_t timeStart, frame_time_t period,
		frame_time_t last,
		CallBackSystem& cbs, 
		const std::function<void(frame_time_t,frame_time_t)>& function);

	void removePendingCallbacks();

};