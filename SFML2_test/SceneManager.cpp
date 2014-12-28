#include "SceneManager.hpp"
#include "GameScene.hpp"

#include <cassert>

void SceneManager::pushState( GameScene& state )
{
	assert(!new_item_to_add || new_item_to_add == &state);
	new_item_to_add = &state;
	if(states.empty()) {
		updateState();
	}
}


void SceneManager::updateState()
{
    if ( to_delete && !states.empty() )
    {
		delete states.top();
		states.pop();
		to_delete = false;
    }

	if( new_item_to_add) {
		states.push(new_item_to_add);
		new_item_to_add->setScene(this, &App);
		new_item_to_add->onInit();
		new_item_to_add = NULL;
	}
}