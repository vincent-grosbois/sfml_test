#include "utils/DrawingUtils.h"

namespace utils {

	void drawRectangle(sf::RenderTexture& texture, const sf::FloatRect& rect, sf::Color color) {
		sf::RectangleShape shape(sf::Vector2f(rect.width, rect.height));
		shape.setPosition(rect.left,rect.top);
		shape.setFillColor(sf::Color(0,0,0,0));
		shape.setOutlineColor(color);
		shape.setOutlineThickness(1);
		texture.draw(shape); 
	}

	void drawPoint(sf::RenderTexture& texture, const sf::Vector2f& point, sf::Color color) {
		sf::RectangleShape shape(sf::Vector2f(1,1));
		shape.setPosition(point);
		shape.setFillColor(sf::Color(0,0,0,0));
		shape.setOutlineColor(color);
		shape.setOutlineThickness(1);
		texture.draw(shape); 
	}
};