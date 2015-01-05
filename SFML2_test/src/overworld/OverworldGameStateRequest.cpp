#include "overworld/OverworldGameStateRequest.h"



OverworldGameStateRequest::OverworldGameStateRequest():
	myPauseRequest(NULL),
	myChangeZCRequest(NULL) { }



bool OverworldGameStateRequest::pushZoneChangeRequest(const std::string& ZC, const sf::Vector2f& position) {
	
	bool erased_previous_state = true;

	if(!myChangeZCRequest) {
		myChangeZCRequest = new ChangeZCRequest();
		erased_previous_state = false;
	}

	myChangeZCRequest->newZC = ZC;
	myChangeZCRequest->position = position;
	return erased_previous_state;
}


ChangeZCRequest* OverworldGameStateRequest::popZoneChangeRequest() {
	if(!myChangeZCRequest) {
		return NULL;
	}

	else {
		ChangeZCRequest* temp = myChangeZCRequest;
		myChangeZCRequest = NULL;
		return temp;
	}
}

