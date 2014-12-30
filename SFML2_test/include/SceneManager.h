#pragma once
#include <stack>

#include <SFML/Graphics.hpp>

class GameScene;


class SceneManager
{

public :

	SceneManager(sf::RenderWindow& App):
		App(App) 
	{ 
		to_delete = false;
		new_item_to_add = NULL;
	}

    GameScene* getCurrentState() { 
		return states.empty() ? NULL : states.top(); 
	}

    void pushState( GameScene& state );

    void updateState();

	sf::RenderWindow& App;

	void removeTop() { to_delete = true; };

private :
	bool to_delete;
	GameScene* new_item_to_add;
    std::stack<GameScene*> states;
};

