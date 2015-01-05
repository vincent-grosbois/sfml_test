#pragma once

#include <SFML/Graphics.hpp>

namespace utils {
	void drawRectangle(sf::RenderTexture& texture, const sf::FloatRect& rect, sf::Color color);
	void drawPoint(sf::RenderTexture& texture, const sf::Vector2f& point, sf::Color color);
};