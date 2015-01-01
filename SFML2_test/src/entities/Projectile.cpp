#include "entities/Projectile.h"

#include "OverWorldScene.h"

Projectile::Projectile(const sf::Vector2f & position,  ZoneContainer& ZC):
	EntityPhysical(position, ZC),
	sides(6),
	lightZone(sf::TrianglesFan, sides+2),
	lightZone2(sf::TrianglesFan, 20+2)
{
	type = EntityType::PROJECTILE;

	boundingBoxSize.x = 20;
	boundingBoxSize.y = 20;

	BoundingBoxRectReal = sf::FloatRect(position.x, position.y, boundingBoxSize.x, boundingBoxSize.y);

	speed = 1.5f*350.f/1000;
	angle = rand();
		
	color = sf::Color(10,10,200, 225);
	radius = 10;

	registerInMaps();
	float deuxpi = float(2*3.14159265);
	float angle = 0.f;
	lightZone[0].position = getPosition();
	lightZone[0].color = color;
	for(int i = 1; i<sides+2; ++i) {
		lightZone[i].position = getPosition() + sf::Vector2f(radius*cos(angle),radius*sin(angle));
		lightZone[i].color = sf::Color(0, 0, 0, 0);
		angle += deuxpi/sides;
	}

	int sides = 20;
	int radius2 = 300.f;
	lightZone2[0].position = getPosition();
	lightZone2[0].color = sf::Color(10, 0, 0, 40);
	for(int i = 1; i<sides+2; ++i) {

		lightZone2[i].position = getPosition() + sf::Vector2f(radius2*cos(angle),radius2*sin(angle));
		lightZone2[i].color = sf::Color(0, 0, 0, 0);

		angle += deuxpi/20;
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

	BoundingBoxRectReal = sf::FloatRect(position.x, position.y, boundingBoxSize.x, boundingBoxSize.y);

	registerInMaps();

	float deuxpi = float(2*3.14159265);
	float angle = 0.f;
	lightZone[0].position = getPosition();
	lightZone2[0].position = getPosition();
	for(int i = 1; i<sides+2; ++i) {
		lightZone[i].position = getPosition() + sf::Vector2f(radius*cos(angle),radius*sin(angle));
		angle += deuxpi/sides;
	}

	float radius2 = 300.f;
	for(int i = 1; i<20+2; ++i) {
		lightZone2[i].position = getPosition() + sf::Vector2f(radius2*cos(angle),radius2*sin(angle));
		angle += deuxpi/20;
	}
}

void Projectile::draw(OverWorldDisplay& owDisplay) {
	owDisplay.light_texture.draw(lightZone, sf::RenderStates(sf::BlendAdd));
	owDisplay.light_texture.draw(lightZone2, sf::RenderStates(sf::BlendAdd));
}

void Projectile::drawCollisionBox(OverWorldDisplay& owDisplay) { 
	sf::RectangleShape rect(sf::Vector2f(BoundingBoxRectReal.width, BoundingBoxRectReal.height));
	rect.setPosition(BoundingBoxRectReal.left,BoundingBoxRectReal.top);
	rect.setFillColor(sf::Color(0,0,0,0));
	rect.setOutlineColor(sf::Color::Blue);
	rect.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect); 
}