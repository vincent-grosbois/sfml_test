#include "overworld/OverWorldScene.h"
#include "ZoneContainerData.h"
#include "overworld/OverWorldTone.h"


void OverWorldDisplay::init(const MetaGameData& metaGameData) {
	colorizeShader.loadFromFile(metaGameData.basePath+"shader/colorize.sfx", sf::Shader::Fragment);

	overWorld_texture.create(metaGameData.resolution.x, metaGameData.resolution.y);
	light_texture.create(metaGameData.resolution.x, metaGameData.resolution.y);

	colorizeShader.setParameter("world_tex", overWorld_texture.getTexture());
	colorizeShader.setParameter("light_tex", light_texture.getTexture());

	world_sprite.setTexture(overWorld_texture.getTexture());
	shader_render_state = sf::RenderStates(&colorizeShader);
}

void OverWorldDisplay::draw(sf::RenderWindow& app) {
	overWorld_texture.display();
	light_texture.display();
	app.draw(world_sprite, shader_render_state);
}

void OverWorldDisplay::updateToneParameters(const Tone& t) {
	colorizeShader.setParameter("Alpha", 1 - t.gray);
	colorizeShader.setParameter("ToneColor", t.red, t.green, t.blue);
}

void OverWorldDisplay::clearAndSetView(const sf::View& view) {
	overWorld_texture.clear(sf::Color::Red);
	light_texture.clear(sf::Color(0,0,0,0));

	overWorld_texture.setView(view);
	light_texture.setView(view);
}