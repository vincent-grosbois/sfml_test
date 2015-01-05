#include "entities/NPC.h"
#include "entities/PlayerCharacter.h"
#include "GameTicks.h"
#include "AI/WaypointModule.h"

DIRECTION::e getOppositeDir(DIRECTION::e dir) {

	if(dir == DIRECTION::UP)
		return DIRECTION::DOWN;
	else if(dir == DIRECTION::DOWN)
		return DIRECTION::UP;
	else if(dir == DIRECTION::LEFT)
		return DIRECTION::RIGHT;
	else if(dir == DIRECTION::RIGHT)
		return DIRECTION::LEFT;

	assert(false);
	return DIRECTION::UP;
}

NPC::NPC(sf::Vector2f const& position,  ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim):
	Character(position,  ZC, ticks, move_anim),
	behavior(NPC_BEHAVIOR::WANDER_NOSTOP)
{
	type = EntityType::NPC;
	this->waypointModule = new WaypointModule();
	waypointModule->active = true;
	waypointModule->loop = true;

	waypointModule->waypoints.push_back(sf::Vector2f(15000,10000));
	waypointModule->waypoints.push_back(sf::Vector2f(15000,12000-500));
}


NPC::~NPC()
{
	//std::cout << "deleting NPC " << this << '\n';
}

bool NPC::moveAtRandom(float value) {

	DIRECTION::e dir;

	if(!isMoving || rand()%100 == 0) 
		dir = (DIRECTION::e)(rand()%4);
	else
		dir = facingDir;

	bool ret = Character::tryMoving(value, dir);

	if(!ret) { isMoving = false ; }

	if(isMoving) {
		facingDir = dir;
	}

	return ret;

}

void NPC::update(int value, bool will_be_drawn)  { 

	if(behavior == NPC_BEHAVIOR::WANDER ){

		if(isMoving) {

			if( rand()%500 != 0 )
				moveAtRandom(value);
			else
				isMoving = false;

		}
		else if ( rand()%100 == 0 ) {
			moveAtRandom(value);
		}

	}
	else if(behavior == NPC_BEHAVIOR::WANDER_NOSTOP ) {
		moveAtRandom(value);
	}
	else if(behavior == NPC_BEHAVIOR::USE_WAYPOINT ) {
		moveToWaypoint(value);
	}
	else if(behavior == NPC_BEHAVIOR::STILL ) {
		//do nothing
	}


}

const int goal_dist = 200;

void NPC::moveToWaypoint(int delta_ms) {

	if(!waypointModule)
		return;

	sf::Vector2f dir;
	while(true) {
		sf::Vector2f& goal = waypointModule->waypoints[waypointModule->current_wp_goal];

		dir = goal - getPosition();

		if( dir.x*dir.x + dir.y*dir.y < goal_dist*goal_dist ) {
			waypointModule->current_wp_goal = (waypointModule->current_wp_goal + 1)%waypointModule->waypoints.size();
		} else {
			break;
		}
	}

	float dir_tot  = fabs(dir.x) + fabs(dir.y);
	dir.x /= dir_tot;
	dir.y /=dir_tot;

	tryMoving(delta_ms*fabs(dir.x), dir.x > 0 ? DIRECTION::RIGHT : DIRECTION::LEFT); 
	tryMoving(delta_ms*fabs(dir.y), dir.y > 0 ? DIRECTION::DOWN : DIRECTION::UP); 
}


bool NPC::onActivated(Entity& activator) { 

	Character* activator_character = dynamic_cast<Character*>(&activator);
	PlayerCharacter* activator_PC = dynamic_cast<PlayerCharacter*>(&activator);

	if(activator_character)
		facingDir = getOppositeDir(activator_character->getFacingDir());

	if(activator_PC) {
		activator_PC->DialogWindow("hurr\ndurr\ndd", true, this);
	}

	return true; 
}

sf::FloatRect NPC::getAwarenessZone() const {
	float advance = 400;
	float width = 300;

	float side_size_x = width/2  - boundingBoxSize.x/2;
	float side_size_y = width/2  - boundingBoxSize.y/2;

	switch(facingDir) {
	case DIRECTION::RIGHT:
		return sf::FloatRect(boundingBoxRectReal.left + boundingBoxRectReal.width, boundingBoxRectReal.top - side_size_y, advance, width);
		break;
	case DIRECTION::LEFT:
		return sf::FloatRect(boundingBoxRectReal.left - advance, boundingBoxRectReal.top -  side_size_y, advance, width);
		break;
	case DIRECTION::DOWN:
		return sf::FloatRect(boundingBoxRectReal.left - side_size_x, boundingBoxRectReal.top + boundingBoxRectReal.height, width, advance);
		break;
	case DIRECTION::UP:
		return sf::FloatRect(boundingBoxRectReal.left - side_size_x, boundingBoxRectReal.top - advance, width, advance);
		break;
	}
	assert(false);
	return sf::FloatRect();
}

void  NPC::drawDebugInfo(OverWorldDisplay& owDisplay)  { 

	sf::RectangleShape rect(sf::Vector2f(boundingBoxRectReal.width, boundingBoxRectReal.height));
	rect.setPosition(boundingBoxRectReal.left,boundingBoxRectReal.top);
	rect.setFillColor(sf::Color(0,0,0,0));
	rect.setOutlineColor(sf::Color::Blue);
	rect.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect); 

	sf::RectangleShape rect2(sf::Vector2f(getAwarenessZone().width, getAwarenessZone().height));
	rect2.setPosition(getAwarenessZone().left,getAwarenessZone().top);
	rect2.setFillColor(sf::Color(0,0,0,0));
	rect2.setOutlineColor(sf::Color::Green);
	rect2.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect2); 

	sf::RectangleShape rect3(sf::Vector2f(1,1));
	rect3.setPosition(getPosition());
	rect3.setFillColor(sf::Color(0,0,0,0));
	rect3.setOutlineColor(sf::Color::Green);
	rect3.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect3); 

	sf::RectangleShape rect4(sf::Vector2f(1,1));
	rect4.setPosition(getSpriteCenter());
	rect4.setFillColor(sf::Color(0,0,0,0));
	rect4.setOutlineColor(sf::Color::Cyan);
	rect4.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect4); 

	if(waypointModule && behavior == NPC_BEHAVIOR::USE_WAYPOINT ) {

		std::vector<sf::Vertex> waypoint(waypointModule->waypoints.size() + 1);
		waypoint[0] = getPosition();
		int i = 1 ;
		for(auto& wp : waypointModule->waypoints) {
			waypoint[i] = sf::Vertex(wp);
			++i;
		}

		owDisplay.overWorld_texture.draw(waypoint.data(), waypointModule->waypoints.size() + 1, sf::LinesStrip);
	}

	sf::RectangleShape rect5(sf::Vector2f(getVisibilityRectangle().width, getVisibilityRectangle().height));
	rect5.setPosition(getVisibilityRectangle().left,getVisibilityRectangle().top);
	rect5.setFillColor(sf::Color(0,0,0,0));
	rect5.setOutlineColor(sf::Color::Yellow);
	rect5.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect5); 
}
