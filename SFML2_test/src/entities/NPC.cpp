#include "entities/NPC.h"
#include "utils/DrawingUtils.h"
#include "GameTicks.h"
#include "AI/WaypointModule.h"

using namespace utils;

DIRECTION::e getOppositeDir(DIRECTION::e dir) {

	switch(dir) {
	case DIRECTION::UP:
		return DIRECTION::DOWN;
	case DIRECTION::DOWN:
		return DIRECTION::UP;
	case DIRECTION::LEFT:
		return DIRECTION::RIGHT;
	case DIRECTION::RIGHT:
		return DIRECTION::LEFT;
	}

	assert(false);
	return DIRECTION::UP;
}

NPC::NPC(sf::Vector2f const& position,  ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim):
	Character(position,  ZC, ticks, move_anim),
	behavior(NpcBehavior::STILL), myState(NpcState::STANDING)
{
	type = EntityType::NPC;
	waypointModule = NULL;
}


NPC::~NPC()
{ }

bool NPC::moveAtRandom(float value) {

	DIRECTION::e dir = facingDir;

	if(myState != NpcState::WALKING || rand()%100 == 0) 
		dir = (DIRECTION::e)(rand()%4);

	bool ret = Character::tryMoving(value, dir);

	if(!ret) {
		myState = NpcState::STANDING;
	} else {
		myState = NpcState::WALKING;
	}

	facingDir = dir;

	return ret;
}

void NPC::update(int value, bool will_be_drawn)  { 

	auto facingDirOld = facingDir;

	if(behavior == NpcBehavior::WANDER ){

		if(myState == NpcState::WALKING) {

			if( rand()%500 != 0 )
				moveAtRandom(value);
			else
				myState = NpcState::STANDING;

		}
		else if ( rand()%100 == 0 ) {
			moveAtRandom(value);
		}

	}
	else if(behavior == NpcBehavior::WANDER_NOSTOP ) {
		moveAtRandom(value);
	}
	else if(behavior == NpcBehavior::USE_WAYPOINT ) {
		moveToWaypoint(value);
	}
	else if(behavior == NpcBehavior::STILL ) {
		//do nothing
	}

	int ticks = this->ticks.getTicks(TICKS::e::_250MS);
	int old_frame = current_frame;
	current_frame = myState == NpcState::WALKING ? (current_frame + ticks) %9 : 0;

	if(old_frame != current_frame || facingDirOld != facingDir ) {
		spriteCpt.sprite.setTextureRect(move_anim->getFrame(facingDir, current_frame ) );
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

	if(activator.getType() == EntityType::NPC)
		facingDir = getOppositeDir(activator_character->getFacingDir());

	return true; 
}

sf::FloatRect NPC::getAwarenessZone() const {
	float advance = 400;
	float width = 300;

	float side_size_x = width/2  - boundingBox.boundingBoxRectReal.width/2;
	float side_size_y = width/2  - boundingBox.boundingBoxRectReal.height/2;

	switch(facingDir) {
	case DIRECTION::RIGHT:
		return sf::FloatRect(boundingBox.boundingBoxRectReal.left + boundingBox.boundingBoxRectReal.width, boundingBox.boundingBoxRectReal.top - side_size_y, advance, width);
		break;
	case DIRECTION::LEFT:
		return sf::FloatRect(boundingBox.boundingBoxRectReal.left - advance, boundingBox.boundingBoxRectReal.top -  side_size_y, advance, width);
		break;
	case DIRECTION::DOWN:
		return sf::FloatRect(boundingBox.boundingBoxRectReal.left - side_size_x, boundingBox.boundingBoxRectReal.top + boundingBox.boundingBoxRectReal.height, width, advance);
		break;
	case DIRECTION::UP:
		return sf::FloatRect(boundingBox.boundingBoxRectReal.left - side_size_x, boundingBox.boundingBoxRectReal.top - advance, width, advance);
		break;
	}
	assert(false);
	return sf::FloatRect();
}



void  NPC::drawDebugInfo(OverworldDisplay& owDisplay)  { 

	drawRectangle(owDisplay.debug_texture, boundingBox.boundingBoxRectReal, sf::Color::Blue);
	drawRectangle(owDisplay.debug_texture,  getAwarenessZone(), sf::Color::Green);
	drawRectangle(owDisplay.debug_texture,  getVisibilityRectangle(), sf::Color::Yellow);
	
	drawPoint(owDisplay.debug_texture, getPosition(), sf::Color::Green);
	drawPoint(owDisplay.debug_texture, getSpriteCenter(), sf::Color::Cyan);

	if(waypointModule && behavior == NpcBehavior::USE_WAYPOINT ) {

		std::vector<sf::Vertex> waypoint(waypointModule->waypoints.size() + 1);
		waypoint[0] = getPosition();
		int i = 1 ;
		for(auto& wp : waypointModule->waypoints) {
			waypoint[i] = sf::Vertex(wp);
			++i;
		}

		owDisplay.debug_texture.draw(waypoint.data(), waypointModule->waypoints.size() + 1, sf::LinesStrip);
	}

}
