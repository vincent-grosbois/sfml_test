#include <iostream>

#include <SFML/Graphics.hpp>

#include "overworld/OverworldScene.h"
#include "SceneManager.h"
#include "TitleMenuScene.h"
#include "ZoneContainerData.h"

#include "GameResource.h"

const int MIN_FPS = 4;


int main()
{

	std::cout << "Build on " << __DATE__ << " at " << __TIME__ << "\n";
#ifdef _DEBUG
	std::cout << "Debug version\n";
#else
	std::cout << "Release version\n";
#endif

	srand(87611);

	MetaGameData metaGame("../../ressources/Game.txt");

	GameResource gameResource(metaGame);

	sf::RenderWindow app(sf::VideoMode(metaGame.resolution.x, metaGame.resolution.y), metaGame.gameTitle);
	app.setFramerateLimit(60);

	SceneManager sceneManager(app);

	GameScene* scene = new OverworldScene(metaGame, gameResource);
	sceneManager.pushState(*scene);
	
	GameScene* currentScene;

	const int max_ms_delta = 1000 / MIN_FPS;

	sf::Clock clock;
	while( (currentScene = sceneManager.getCurrentState()) && app.isOpen() ) {

		sf::Time elapsed = clock.restart();

		auto ms_delta = elapsed.asMilliseconds();

		if(ms_delta == 0) {
			ms_delta = 1;
		}
		else if(ms_delta>max_ms_delta) {
			ms_delta = max_ms_delta;
		}

		currentScene->update(ms_delta);

		currentScene->draw();

		sceneManager.updateState();
	}

	std::cout << "exiting...\n";

	return EXIT_SUCCESS;
}