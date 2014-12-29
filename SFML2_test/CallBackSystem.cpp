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

void CallBackInfo::call(time_tt original_time, time_tt actual_time) {
	scoped_bool scoped_bool_instance(isExecuting);

	function(original_time, actual_time);
	targetCallbackList.erase(targetCallbackPosition);
}


CallBackSystem::CallBackSystem() {

}

void CallBackSystem::callAllUpToTime(time_tt time) {

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

	for(auto it = events.begin(); it != events.end();  ) {
		for(auto& callback : it->second) {
			callback.targetCallbackList.erase(callback.targetCallbackPosition);
		}
	}

}


ActiveCallBackInfo CallBackSystem::addCallback(time_tt time, const std::function<void(time_tt,time_tt)>& function, std::list<ActiveCallBackInfo>& targetCallbackList) {

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
	for(auto& activeCallback : activeCallbacks) {
		activeCallback.list.erase(activeCallback.callbackPosition);
	}
}

void CallBackReceiver::createCallback(time_tt time, CallBackSystem& cbs, const std::function<void(time_tt,time_tt)>& function) {
	auto callbackInfo = cbs.addCallback(time, function, activeCallbacks);
	activeCallbacks.push_back(callbackInfo);
	callbackInfo.callbackPosition->targetCallbackPosition = --activeCallbacks.end();
}


void function_repeat(time_tt t1, time_tt t2, 
					 const std::function<void(time_tt,time_tt)>& function,
					 time_tt period,
					 CallBackSystem& cbs,
					 CallBackReceiver& cbr)
{
	function(t1, t2);
	cbr.createPeriodicCallback(t1 + period, period, cbs, function);
}

void function_repeat_until(time_tt t1, time_tt t2,
					 const std::function<void(time_tt,time_tt)>& function,
					 time_tt period, time_tt last,
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

void CallBackReceiver::createPeriodicCallback(time_tt timeStart, time_tt period,
											  CallBackSystem& cbs, 
											  const std::function<void(time_tt,time_tt)>& function)
{
	assert(period > 0);

	using namespace std::placeholders;
	auto function2 = std::bind(function_repeat, _1, _2, function, period, std::ref(cbs), std::ref(*this));

	createCallback(timeStart, cbs, function2);
}

void CallBackReceiver::createPeriodicCallback(time_tt timeStart, time_tt period, time_tt last,
											  CallBackSystem& cbs, 
											  const std::function<void(time_tt,time_tt)>& function)
{
	assert(period > 0);

	using namespace std::placeholders;
	auto function2 = std::bind(function_repeat_until, _1, _2, function, period, last, std::ref(cbs), std::ref(*this));

	createCallback(timeStart, cbs, function2);
}