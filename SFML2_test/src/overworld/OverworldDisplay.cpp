#include "overworld/OverWorldScene.h"
#include "ZoneContainerData.h"
#include "overworld/OverWorldTone.h"
#include "utils/AngleUtils.h"

void OverWorldDisplay::init(const MetaGameData& metaGameData) {

	colorizeShader.loadFromFile(metaGameData.basePath+"shader/colorize.sfx", sf::Shader::Fragment);
	waterShader.loadFromFile(metaGameData.basePath+"shader/wave.vert", sf::Shader::Vertex);
	changeWaterParameters(myWaveParameters);

	overWorld_texture.create(metaGameData.resolution.x, metaGameData.resolution.y);
	light_texture.create(metaGameData.resolution.x, metaGameData.resolution.y);
	debug_texture.create(metaGameData.resolution.x, metaGameData.resolution.y);

	colorizeShader.setParameter("world_tex", overWorld_texture.getTexture());
	colorizeShader.setParameter("light_tex", light_texture.getTexture());

	world_sprite.setTexture(overWorld_texture.getTexture());
	debug_sprite.setTexture(debug_texture.getTexture());

	shader_render_state = sf::RenderStates(&colorizeShader);
}

void OverWorldDisplay::draw(sf::RenderWindow& app) {
	overWorld_texture.display();
	light_texture.display();
	debug_texture.display();
	app.draw(world_sprite, shader_render_state);
	app.draw(debug_sprite);
}

void OverWorldDisplay::updateToneParameters(const Tone& t) {
	colorizeShader.setParameter("Alpha", 1 - t.gray);
	colorizeShader.setParameter("ToneColor", t.red, t.green, t.blue);
}


WaveParameters::WaveParameters() :
	wave_angle(utils::angles::degrees2rad(-45)),
	wave_time_period(2.f*4),
	wave_spatial_period(32*5.f),
	wave_amplitude(7.f) {
}

void OverWorldDisplay::updateWaterParameters(int time_ms) {

	float time_phase = 2*utils::angles::pi*time_ms/(myWaveParameters.wave_time_period*1000.f);

	waterShader.setParameter("waveTimePhase", time_phase);
}

void OverWorldDisplay::changeWaterParameters(const WaveParameters& waveParameters) {

	myWaveParameters = waveParameters;

	float dir_x = 2*utils::angles::pi * cos(waveParameters.wave_angle)/ waveParameters.wave_spatial_period;
	float dir_y = 2*utils::angles::pi *-sin(waveParameters.wave_angle)/ waveParameters.wave_spatial_period;

	waterShader.setParameter("waveInfo", dir_x,  dir_y, waveParameters.wave_amplitude);
}

void OverWorldDisplay::clearAndSetView(const sf::View& view) {
	overWorld_texture.clear(sf::Color::Red);
	light_texture.clear(sf::Color(0,0,0,0));
	debug_texture.clear(sf::Color(0,0,0,0));

	overWorld_texture.setView(view);
	light_texture.setView(view);
	debug_texture.setView(view);
}