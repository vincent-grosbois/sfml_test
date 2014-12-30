#pragma once

#include <SFML/Graphics.hpp>

#include "SceneManager.h"


class GameScene
{
public:
	GameScene() { };

	virtual ~GameScene() { };

	virtual void onInit () { };

	virtual void update (int deltatime_ms) { };

	virtual void draw () = 0;

	virtual void onClose() { };

	void close() { 
		scene->removeTop();
		onClose(); 
	}


	void pushNewScene(GameScene* newScene) {
		scene->pushState(*newScene);
	}

	void setScene(SceneManager* _scene, sf::RenderWindow* _App) { scene = _scene; App = _App; };

private: 
	SceneManager* scene;

protected:
	sf::RenderWindow* App;

};

