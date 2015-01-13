#include "entities/Projectile.h"

#include "overworld/OverworldScene.h"


#include "utils/DrawingUtils.h"

using namespace utils;
using namespace utils::angles;



LightEmitterCpt::LightEmitterCpt(const sf::Vector2f& pos, int sides, float radius, sf::Color& color):
	lightZone(sf::TrianglesFan, sides+2),sides(sides), radius(radius), color(color)
{
	rebuildVertexArray(pos);
}

void LightEmitterCpt::moveVertexArray(const sf::Vector2f& pos) {
	float angle = 0.f;
	lightZone[0].position = pos;
	for(int i = 1; i<sides+2; ++i) {
		lightZone[i].position = pos + sf::Vector2f(radius*cos(angle),radius*sin(angle));
		angle += 2*utils::angles::pi/sides;
	}
}

void LightEmitterCpt::rebuildVertexArray(const sf::Vector2f& pos) {
	float angle = 0.f;
	lightZone[0].position = pos;
	lightZone[0].color = color;
	for(int i = 1; i<sides+2; ++i) {
		lightZone[i].position = pos + sf::Vector2f(radius*cos(angle),radius*sin(angle));
		lightZone[i].color = sf::Color(0, 0, 0, 0);
		angle += 2*utils::angles::pi/sides;
	}
}

void LightEmitterCpt::recolorVertexArray(const sf::Vector2f& pos) {
	lightZone[0].color = color;
	for(int i = 1; i<sides+2; ++i) {
		lightZone[i].color = sf::Color(0, 0, 0, 0);
	}
}


void LightEmitterCpt::draw(OverworldDisplay& owDisplay) const {
	owDisplay.light_texture.draw(lightZone, sf::RenderStates(sf::BlendAdd));
}


Projectile::Projectile(const sf::Vector2f& position,  ZoneContainer& ZC, DIRECTION::e dir):
	EntityPhysical(position, ZC),
	light1(position, 8, 10, sf::Color(10,10,200, 225)),
	light2(position, 20, 300, sf::Color(10, 0, 0, 40))
{
	lifetime_ms = 100*1000;

	type = EntityType::PROJECTILE;
	
	boundingBox.boundingBoxOffset.x = -10;
	boundingBox.boundingBoxOffset.y = -10;
	boundingBox.boundingBoxRectReal.width = 20;
	boundingBox.boundingBoxRectReal.height = 20;


	boundingBox.positionBoundingBox(position);
	
	speed = 1.f*350.f/1000;
	angle = dir2angle(dir);
			

	registerInMaps();
}

sf::FloatRect Projectile::getVisibilityRectangle() const
{ return sf::FloatRect(position.x-150, position.y-150, 300, 300); };


Projectile::~Projectile(void)
{
	//std::cout << "projectile removed\n";
}

void Projectile::update(int delta_ms, bool will_be_drawn) {
	position.x += speed*delta_ms*cos(angle);
	position.y += speed*delta_ms*sin(angle);

	boundingBox.positionBoundingBox(position);


	registerInMaps();

	light1.moveVertexArray(position);
	light2.moveVertexArray(position);

	lifetime_ms -= delta_ms;
	if(lifetime_ms < 0) {
		markForDeletion();
	}
}

void Projectile::draw(OverworldDisplay& owDisplay) {
	light1.draw(owDisplay);
	light2.draw(owDisplay);
}

void Projectile::drawDebugInfo(OverworldDisplay& owDisplay) { 
	drawRectangle(owDisplay.debug_texture, boundingBox.boundingBoxRectReal, sf::Color::Blue);
	drawRectangle(owDisplay.debug_texture, getVisibilityRectangle(), sf::Color::Yellow);
}