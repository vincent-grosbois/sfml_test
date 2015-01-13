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

	bool hasZoneChangeRequest() const;
	bool hasPauseRequest() const;

	const ChangeZCRequest& getZoneChangeRequest() const;
	const PauseRequest& getPauseRequest() const;

	void clear();

private:
	ChangeZCRequest myChangeZCRequest;
	bool changeZCRequest_;

	PauseRequest	myPauseRequest;
	bool pauseRequest_;

	OverworldGameStateRequest(const OverworldGameStateRequest&);
	OverworldGameStateRequest& operator=(OverworldGameStateRequest&);
};