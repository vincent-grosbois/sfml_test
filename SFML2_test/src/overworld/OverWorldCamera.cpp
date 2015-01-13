#include "overworld/OverworldCamera.h"

#include <cassert>

bool OverworldCamera::movedSinceLastFrame() const {
	return !same_as_last_view;
}

void OverworldCamera::newFrame() {
	previous_view = current_view;
	previous_view_rect = current_view_rect;
	same_as_last_view = true;
	camera_set_for_frame = false;
}

void OverworldCamera::cameraSetForFrame() {
	current_view_rect = 
		sf::FloatRect(
		current_view.getCenter() - sf::Vector2f(current_view.getSize().x/2, current_view.getSize().y/2) ,
		current_view.getSize()
		);

	camera_set_for_frame = true;
}

const sf::View& OverworldCamera::getView() const {
	return current_view;
}

const sf::FloatRect& OverworldCamera::getViewRect() const {
	assert(camera_set_for_frame);
	return current_view_rect;
}

const sf::View& OverworldCamera::getPreviousView() const {
	return previous_view;
}

void OverworldCamera::setCenter(const sf::Vector2f& center) {
	if(current_view.getCenter() == center) 
		return;

	current_view.setCenter(center);
	same_as_last_view = false;
}

sf::Vector2f OverworldCamera::getCenter() const {
	return current_view.getCenter();
}

float OverworldCamera::getZoom() const {
	return current_zoom;
}

void OverworldCamera::zoom(float value) {
	current_zoom /= value;
	if(current_zoom > max_zoom || current_zoom < min_zoom) {
		current_zoom *= value;
	} else {
		current_view.zoom(1.f/value);
		same_as_last_view = false;
	}
}

void OverworldCamera::resetSize() {
	current_view.setSize(defaultSize);
	same_as_last_view = false;
	current_zoom = 1.f;
}

OverworldCamera::OverworldCamera(const sf::Vector2f& defaultSize):
	current_view(sf::Vector2f(0,0), defaultSize),
	same_as_last_view(false),
	camera_set_for_frame(false),
	current_zoom(1.f),
	min_zoom(0.15f),
	max_zoom(10.f),
	defaultSize(defaultSize)
{ }

OverworldCamera::OverworldCamera(const sf::Vector2i& defaultSize):
	current_view(
	sf::Vector2f(0,0), 
	sf::Vector2f(static_cast<float>(defaultSize.x), static_cast<float>(defaultSize.y))
	),
	same_as_last_view(false),
	camera_set_for_frame(false),
	current_zoom(1.f),
	min_zoom(0.15f),
	max_zoom(10.f),
	defaultSize(defaultSize)
{ }

