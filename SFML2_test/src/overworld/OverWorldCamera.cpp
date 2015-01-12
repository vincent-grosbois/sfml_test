#include "overworld/OverWorldCamera.h"

#include <cassert>

bool OverWorldCamera::movedSinceLastFrame() const {
	return !same_as_last_view;
}

void OverWorldCamera::newFrame() {
	previous_view = current_view;
	previous_view_rect = current_view_rect;
	same_as_last_view = true;
	camera_set_for_frame = false;
}

void OverWorldCamera::cameraSetForFrame() {
	current_view_rect = 
		sf::FloatRect(
		current_view.getCenter() - sf::Vector2f(current_view.getSize().x/2, current_view.getSize().y/2) ,
		current_view.getSize()
		);

	camera_set_for_frame = true;
}

const sf::View& OverWorldCamera::getView() const {
	return current_view;
}

const sf::FloatRect& OverWorldCamera::getViewRect() const {
	assert(camera_set_for_frame);
	return current_view_rect;
}

const sf::View& OverWorldCamera::getPreviousView() const {
	return previous_view;
}

void OverWorldCamera::setCenter(const sf::Vector2f& center) {
	if(current_view.getCenter() == center) 
		return;

	current_view.setCenter(int(center.x), int(center.y));
	same_as_last_view = false;
}

sf::Vector2f OverWorldCamera::getCenter() const {
	return current_view.getCenter();
}

float OverWorldCamera::getZoom() const {
	return current_zoom;
}

void OverWorldCamera::zoom(float value) {
	current_zoom *= value;
	if(current_zoom > max_zoom || current_zoom < min_zoom) {
		current_zoom /= value;
	} else {
		/*auto size = current_view.getSize();
		size.x = int(size.x * value);
		size.y = int(size.y * value);


		size.x = defaultSize.x * (int(32*size.x) / int(defaultSize.x) ) / 32.f;
		size.y = defaultSize.y * (int(32*size.y) / int(defaultSize.y) ) / 32.f;

		current_view.setSize(size);
		*/


		current_view.zoom(value);
		same_as_last_view = false;
	}
}

void OverWorldCamera::resetSize() {
	current_view.setSize(defaultSize);
	same_as_last_view = false;
	current_zoom = 1.f;
}

OverWorldCamera::OverWorldCamera(const sf::Vector2f& defaultSize):
	current_view(sf::Vector2f(0,0), defaultSize),
	same_as_last_view(false),
	camera_set_for_frame(false),
	current_zoom(1.f),
	min_zoom(0.15f),
	max_zoom(10.f),
	defaultSize(defaultSize)
{ }

OverWorldCamera::OverWorldCamera(const sf::Vector2i& defaultSize):
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

