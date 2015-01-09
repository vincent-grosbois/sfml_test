#include "Overlay.h"



Overlay::Overlay(sf::RenderWindow& App):
App(App),
MapInfosText(),
overlayScene(NULL)
{

	myFont.loadFromFile("../../ressources/FuturaExtended.ttf");

	FPStext.setFont(myFont);
	FPStext.setPosition(0, 0);
	FPStext.setColor(sf::Color::White);
	FPStext.setCharacterSize(20);

	////
	MapInfosText.setFont(myFont);
	MapInfosText.setPosition(0, 22*3);
	MapInfosText.setColor(sf::Color::White);
	MapInfosText.setCharacterSize(15);

	//
	//map.loadFromFile("../../ressources/map.png");
	mapSprite = sf::Sprite(map);
	mapSprite.setScale(2.f/3,2.f/3);
	
	
	cursor.loadFromFile("../../ressources/here.png");
	cursorSprite = sf::Sprite(cursor);
}

Overlay::~Overlay() {
}


void Overlay::draw(bool DrawMap) {
	App.draw(FPStext);
	App.draw(MapInfosText);

	if(DrawMap) {

		App.draw(mapSprite);

		//cursorSprite.setPosition(OverWorldGameState::getPlayer()->getSpriteCenter().x / (32*3*3/2) - 1, OverWorldGameState::getPlayer()->getSpriteCenter().y / (32*3*3/2) - 1);

		App.draw(cursorSprite);

		//const sf::View& v= ZoneContainer::worldTexture->getView();

		//sf::FloatRect rect((v.getCenter().x - v.getSize().x/2)/(32*3*3/2), (v.getCenter().y - v.getSize().y/2)/(32*3*3/2), v.getSize().x/(32*3*3/2), v.getSize().y/(32*3*3/2));

		//App.draw(sf::Shape::Rectangle( rect,sf::Color(0,0,0,0), 1, sf::Color::Green));
	}

	if(overlayScene) {
		overlayScene->Update();
		overlayScene->draw();
	}
}
