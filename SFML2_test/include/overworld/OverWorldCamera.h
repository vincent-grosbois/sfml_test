#pragma once

#include <SFML/Graphics/View.hpp>

class OverWorldCamera {
public:
	OverWorldCamera(const sf::Vector2f& defaultSize);
	OverWorldCamera(const sf::Vector2i& defaultSize);

	bool movedSinceLastFrame() const;
	void newFrame();
	void cameraSetForFrame();
	const sf::View& getView() const;
	const sf::FloatRect& getViewRect() const;
	const sf::View& getPreviousView() const;
	void setCenter(const sf::Vector2f& center);
	sf::Vector2f getCenter() const;
	void zoom(float value);
	void resetSize();
	float getZoom() const;

private:
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
};