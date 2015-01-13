#pragma once

#include <SFML/Graphics/View.hpp>

class OverworldCamera {
public:
	OverworldCamera(const sf::Vector2f& defaultSize);
	OverworldCamera(const sf::Vector2i& defaultSize);

	bool movedSinceLastFrame() const;

	void newFrame();
	void cameraSetForFrame();

	const sf::View& getView() const;
	const sf::View& getPreviousView() const;

	const sf::FloatRect& getViewRect() const;

	void setCenter(const sf::Vector2f& center);
	sf::Vector2f getCenter() const;

	void zoom(float value);
	float getZoom() const;

	void resetSize();

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