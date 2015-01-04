#include "entities/Projectile.h"

#include "OverWorldScene.h"

#include "utils/AngleUtils.h"

using namespace utils;


Projectile::Projectile(const sf::Vector2f & position,  ZoneContainer& ZC, DIRECTION::e dir):
	EntityPhysical(position, ZC),
	sides(6),
	lightZone(sf::TrianglesFan, sides+2),
	lightZone2(sf::TrianglesFan, 20+2)
{
	lifetime_ms = 100*1000;

	type = EntityType::PROJECTILE;
	
	boundingBoxOffset.x = -10;
	boundingBoxOffset.y = -10;

	boundingBoxSize.x = 20;
	boundingBoxSize.y = 20;

	boundingBoxRectReal = sf::FloatRect(position.x + boundingBoxOffset.x, position.y + boundingBoxOffset.y, boundingBoxSize.x, boundingBoxSize.y);

	speed = 1.5f*350.f/1000;
	angle = dir2angle(dir);
		
	color = sf::Color(10,10,200, 225);
	radius = 10;

	registerInMaps();
	float deuxpi = float(2*3.14159265);
	float my_angle = 0.f;
	lightZone[0].position = getPosition();
	lightZone[0].color = color;
	for(int i = 1; i<sides+2; ++i) {
		lightZone[i].position = getPosition() + sf::Vector2f(radius*cos(my_angle),radius*sin(my_angle));
		lightZone[i].color = sf::Color(0, 0, 0, 0);
		my_angle += deuxpi/sides;
	}

	int sides = 20;
	int radius2 = 300;
	lightZone2[0].position = getPosition();
	lightZone2[0].color = sf::Color(10, 0, 0, 40);
	for(int i = 1; i<sides+2; ++i) {

		lightZone2[i].position = getPosition() + sf::Vector2f(radius2*cos(my_angle),radius2*sin(my_angle));
		lightZone2[i].color = sf::Color(0, 0, 0, 0);

		my_angle += deuxpi/20;
	}
}

sf::FloatRect Projectile::getVisibilityRectangle() const
{ return sf::FloatRect(position.x-150, position.y-150, 300, 300); };


Projectile::~Projectile(void)
{
	//std::cout << "projectile removed\n";
}

void Projectile::update(int delta_ms) {
	position.x += speed*delta_ms*cos(angle);
	position.y += speed*delta_ms*sin(angle);

	boundingBoxRectReal = sf::FloatRect(position.x + boundingBoxOffset.x, position.y + boundingBoxOffset.y, boundingBoxSize.x, boundingBoxSize.y);

	registerInMaps();

	float deuxpi = float(2*3.14159265);
	float my_angle = 0.f;
	lightZone[0].position = getPosition();
	lightZone2[0].position = getPosition();
	for(int i = 1; i<sides+2; ++i) {
		lightZone[i].position = getPosition() + sf::Vector2f(radius*cos(my_angle),radius*sin(my_angle));
		my_angle += deuxpi/sides;
	}

	float radius2 = 300.f;
	for(int i = 1; i<20+2; ++i) {
		lightZone2[i].position = getPosition() + sf::Vector2f(radius2*cos(my_angle),radius2*sin(my_angle));
		my_angle += deuxpi/20;
	}

	lifetime_ms -= delta_ms;
	if(lifetime_ms < 0) {
		markForDeletion();
	}
}

void Projectile::draw(OverWorldDisplay& owDisplay) {
	owDisplay.light_texture.draw(lightZone, sf::RenderStates(sf::BlendAdd));
	owDisplay.light_texture.draw(lightZone2, sf::RenderStates(sf::BlendAdd));
}

void Projectile::drawDebugInfo(OverWorldDisplay& owDisplay) { 
	sf::RectangleShape rect(sf::Vector2f(boundingBoxRectReal.width, boundingBoxRectReal.height));
	rect.setPosition(boundingBoxRectReal.left,boundingBoxRectReal.top);
	rect.setFillColor(sf::Color(0,0,0,0));
	rect.setOutlineColor(sf::Color::Blue);
	rect.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect); 
}