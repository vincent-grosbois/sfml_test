#pragma once

#include <cassert>

#include <SFML/Graphics/View.hpp>

class OverWorldCamera {
	sf::View current_view;
	sf::View previous_view;

	sf::FloatRect current_view_rect;
	sf::FloatRect previous_view_rect;

	bool same_as_last_view;
	bool camera_set_for_frame;

	float current_zoom;
	float min_zoom;
	float max_zoom;

	sf::Vector2f defaultSize;

public:

	bool movedSinceLastFrame() const {
		return !same_as_last_view;
	}

	void newFrame() {
		previous_view = current_view;
		previous_view_rect = current_view_rect;
		same_as_last_view = true;
		camera_set_for_frame = false;
	}

	void cameraSetForFrame() {
		current_view_rect = 
			sf::FloatRect(
			current_view.getCenter() - sf::Vector2f(current_view.getSize().x/2, current_view.getSize().y/2) ,
			current_view.getSize()
			);

		camera_set_for_frame = true;
	}

	const sf::View& getView() const {
		return current_view;
	}

	const sf::FloatRect& getViewRect() const {
		assert(camera_set_for_frame);
		return current_view_rect;
	}

	const sf::View& getPreviousView() const {
		return previous_view;
	}

	void setCenter(const sf::Vector2f& center) {
		if(current_view.getCenter() == center) 
			return;

		current_view.setCenter(center);
		same_as_last_view = false;
	}

	void zoom(float value) {
		current_zoom *= value;
		if(current_zoom > max_zoom || current_zoom < min_zoom) {
			current_zoom /= value;
		} else {
			current_view.zoom(value);
			same_as_last_view = false;
		}
	}

	void resetSize() {
		current_view.setSize(defaultSize);
		same_as_last_view = false;
		current_zoom = 1.f;
	}

	OverWorldCamera(const sf::Vector2f& defaultSize):
		current_view(sf::Vector2f(0,0), defaultSize),
		same_as_last_view(false),
		camera_set_for_frame(false),
		current_zoom(1.f),
		min_zoom(0.3f),
		max_zoom(7.f),
		defaultSize(defaultSize)
	{ }

};