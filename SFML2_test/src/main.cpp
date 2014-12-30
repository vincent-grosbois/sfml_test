#include <iostream>

#include <SFML/Graphics.hpp>

#include "OverWorldScene.h"
#include "SceneManager.h"
#include "TitleMenuScene.h"
#include "ZoneContainerData.h"

#include "GameResource.h"




int main()
{
	srand(87611);

	MetaGameData metaGame("../../ressources/Game.txt");

	GameResource gameResource;

	sf::RenderWindow app(sf::VideoMode(metaGame.resolution.x, metaGame.resolution.y), metaGame.title);
	app.setFramerateLimit(60);

	SceneManager sceneManager(app);

	GameScene* scene = new OverWorldScene(metaGame, gameResource);
	sceneManager.pushState(*scene);
	
	GameScene* currentScene;

	sf::Clock clock;
	while( (currentScene = sceneManager.getCurrentState()) && app.isOpen() ) {

		 sf::Time elapsed = clock.restart();

		currentScene->update(elapsed.asMilliseconds() == 0 ? 1 : elapsed.asMilliseconds());

		currentScene->draw();

		sceneManager.updateState();
	}

	std::cout << "exiting...\n";

	return EXIT_SUCCESS;
}