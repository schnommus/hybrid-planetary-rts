#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <Artemis/Artemis.h>

#include "Game.h"
#include "ProcessingSystem.h"

class SelectionSystem: public artemis::ProcessingSystem {
public:
	SelectionSystem(Game &gamev);
	void drawUnder();
	void drawOver();
protected:
	virtual void initialize();
	virtual void doProcessing();
	bool isInsideRectangle( int x, int y, int x1, int y1, int x2, int y2 );
private:
	Game &game;
	sf::Text desc;
	std::vector< artemis::Entity * > selectedEntities;
	sf::Vector2i boxOrigin;
	bool loopSelecting;
};
