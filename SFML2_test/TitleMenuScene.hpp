#pragma once
#include "gamescene.hpp"
#include "OverWorldScene.hpp"

#include <iostream>

class TitleMenuScene : public GameScene
{
public:
	TitleMenuScene(std::string text2) :  text()//text2)
	{
		text.setPosition(20,30);

		color = sf::Color(rand()%250, rand()%250, rand()%250);

	};

	virtual void draw() override { App->clear(color); App->draw(text); App->display(); App->draw(sf::Sprite(sheet));};

	virtual void update( int deltatime_ms ) override { 
		sf::Event Event;
		while (App->pollEvent(Event))
		{

			// Close window : exit
			if (Event.type == sf::Event::Closed) {
				close();
			}

			if(Event.type == sf::Event::KeyPressed) {
				//pushNewScene(new OverWorldScene("") );
			}

		};

		//text.draw( rand()%3-1, 0);

	};


	virtual void onClose() override{  std::cout << "closed " << this << color.r <<"\n";};
	virtual void onInit() override{  

		
	

	if (!sheet_img.loadFromFile("../ressources/Autotiles/1.png")) 
		std::cout << "could not load "<< std::endl;
	else
		std::cout << "\nLoaded\n";


	
	sheet.loadFromImage(sheet_img);

		std::cout << "Init "<< this << " with "<< color.r <<"\n";
	
	};
	sf::Image  sheet_img;
	sf::Text text;
	sf::Color color;
	sf::Texture sheet;
};

