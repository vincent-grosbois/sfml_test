#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <vector>

class BoxedText
{
public:
	BoxedText(const sf::Rect<float>& theBox, float textSize);
	bool setText(const std::string& theText);
	bool isTooBig() const { return tooBig ; };
	void draw(sf::RenderWindow& App) { 

		if(highlight) {
		//visualText.draw(highlight_shift);
		visualText.setColor(highlight_color);
		App.draw(visualText);
		visualText.move(-highlight_shift);
		visualText.setColor(sf::Color(255,255,255));
		}
		App.draw(visualText);
		
	};

	bool highlight;
	sf::Vector2f highlight_shift;
	sf::Color highlight_color;

protected:
	std::string myText;
	sf::Text visualText;
	sf::Rect<float> myBox;
	bool tooBig;
	void SplitTextForWidth();
	virtual void SplitTextForHeight();

};

