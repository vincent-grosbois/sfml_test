#include "overworld/OverworldGameStateRequest.h"



OverworldGameStateRequest::OverworldGameStateRequest():
	changeZCRequest_(false),
	pauseRequest_(false) { }



bool OverworldGameStateRequest::pushZoneChangeRequest(const std::string& ZC, const sf::Vector2f& position) {
	bool erased_previous_state = changeZCRequest_;
	myChangeZCRequest.newZC = ZC;
	myChangeZCRequest.position = position;
	changeZCRequest_ = true;
	return erased_previous_state;
}


bool OverworldGameStateRequest::pushPauseRequest(bool pause) {
	bool erased_previous_state = pauseRequest_;
	myPauseRequest.pause = pause;
	pauseRequest_ = true;
	return erased_previous_state;
}

bool OverworldGameStateRequest::hasZoneChangeRequest() const {
	return changeZCRequest_;
}

bool OverworldGameStateRequest::hasPauseRequest() const {
	return pauseRequest_;
}

const ChangeZCRequest& OverworldGameStateRequest::getZoneChangeRequest() const {
	return myChangeZCRequest;
}

const PauseRequest& OverworldGameStateRequest::getPauseRequest() const {
	return myPauseRequest;
}

void OverworldGameStateRequest::clear() {
	changeZCRequest_ = false;
	pauseRequest_ = false;
}
