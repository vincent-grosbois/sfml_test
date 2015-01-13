#include "CallBackSystem.h"

#include <cassert>

struct scoped_bool {
	bool& value;
	scoped_bool(bool& value) :
		value(value)
	{
		value = true;
	}
	~scoped_bool() {
		value = false;
	}
};

CallBackInfo::CallBackInfo(std::list<ActiveCallBackInfo>& targetCallbackList):
	targetCallbackList(targetCallbackList),
	isExecuting(false)
{ }

void CallBackInfo::call(frame_time_t original_time, frame_time_t actual_time) {
	scoped_bool scoped_bool_instance(isExecuting);

	function(original_time, actual_time);
	targetCallbackList.erase(targetCallbackPosition);
}


CallBackSystem::CallBackSystem() {

}

void CallBackSystem::callAllUpToTime(frame_time_t time) {

	for(auto it = events.begin(); it != events.end();  ) {

		if(it->first > time) 
			return;

		for(auto& callback : it->second) {
			callback.call(it->first, time);
		}

		events.erase(it++);
	}
}

CallBackSystem::~CallBackSystem() {

	//unbind all callbacks

	for(auto it = events.begin(); it != events.end();  ++it) {
		for(auto& callback : it->second) {
			callback.targetCallbackList.erase(callback.targetCallbackPosition);
		}
	}

}


ActiveCallBackInfo CallBackSystem::addCallback
	(frame_time_t time, const std::function<void(frame_time_t,frame_time_t)>& function, 
	std::list<ActiveCallBackInfo>& targetCallbackList) {

	std::list<CallBackInfo>& event_list = events[time];

	event_list.emplace_back(CallBackInfo(targetCallbackList));
	event_list.back().function = function;

	return ActiveCallBackInfo(event_list, --event_list.end());
}


ActiveCallBackInfo::ActiveCallBackInfo(std::list<CallBackInfo>& list, std::list<CallBackInfo>::iterator& pos) :
	list(list),
	callbackPosition(pos)
{ }

CallBackReceiver::~CallBackReceiver() {
	removePendingCallbacks();
}

void CallBackReceiver::createCallback(frame_time_t time, CallBackSystem& cbs, const std::function<void(frame_time_t,frame_time_t)>& function) {
	auto callbackInfo = cbs.addCallback(time, function, activeCallbacks);
	activeCallbacks.push_back(callbackInfo);
	callbackInfo.callbackPosition->targetCallbackPosition = --activeCallbacks.end();
}


void function_repeat(frame_time_t t1, frame_time_t t2, 
					 const std::function<void(frame_time_t,frame_time_t)>& function,
					 frame_time_t period,
					 CallBackSystem& cbs,
					 CallBackReceiver& cbr)
{
	function(t1, t2);
	cbr.createPeriodicCallback(t1 + period, period, cbs, function);
}

void function_repeat_until(frame_time_t t1, frame_time_t t2,
					 const std::function<void(frame_time_t,frame_time_t)>& function,
					 frame_time_t period, frame_time_t last,
					 CallBackSystem& cbs,
					 CallBackReceiver& cbr)
{
	if(t1 <= last) {
		function(t1, t2);
		if(t1 + period <= last) {
			cbr.createPeriodicCallback(t1 + period, period, last, cbs, function);
		}
	}
}

void CallBackReceiver::createPeriodicCallback(frame_time_t timeStart, frame_time_t period,
											  CallBackSystem& cbs, 
											  const std::function<void(frame_time_t,frame_time_t)>& function)
{
	assert(period > 0);

	using namespace std::placeholders;
	auto repeated_function = std::bind(function_repeat, _1, _2, function, period, std::ref(cbs), std::ref(*this));

	createCallback(timeStart, cbs, repeated_function);
}

void CallBackReceiver::createPeriodicCallback(frame_time_t timeStart, frame_time_t period, frame_time_t last,
											  CallBackSystem& cbs, 
											  const std::function<void(frame_time_t,frame_time_t)>& function)
{
	assert(period > 0);

	using namespace std::placeholders;
	auto repeated_function = std::bind(function_repeat_until, _1, _2, function, period, last, std::ref(cbs), std::ref(*this));

	createCallback(timeStart, cbs, repeated_function);
}

void CallBackReceiver::removePendingCallbacks()
{
	for(auto& activeCallback : activeCallbacks) {
		activeCallback.list.erase(activeCallback.callbackPosition);
	}
	activeCallbacks.clear();
}