#pragma once

#include <string>
#include <SFML/System/Vector2.hpp>

struct ChangeZCRequest {
	std::string newZC;
	sf::Vector2f position;
};

struct PauseRequest {
	bool pause;
};

class OverworldGameStateRequest {
public:
	OverworldGameStateRequest();

	bool pushZoneChangeRequest(const std::string& ZC, const sf::Vector2f& position);
	bool pushPauseRequest(bool pause);

	ChangeZCRequest* popZoneChangeRequest();
	PauseRequest* popPauseRequest();

private:
	ChangeZCRequest* myChangeZCRequest;
	PauseRequest*	myPauseRequest;

	OverworldGameStateRequest(const OverworldGameStateRequest&);
	OverworldGameStateRequest& operator=(OverworldGameStateRequest&);
};