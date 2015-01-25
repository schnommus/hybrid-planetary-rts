#pragma once

#include "ProcessingSystem.h"
#include "Game.h"
#include <SFML/Graphics.hpp>

class DrawDebugInfoSystem : public artemis::ProcessingSystem {
public:
	DrawDebugInfoSystem (Game &gamev);
protected:
	virtual void doProcessing ();
	virtual void initialize ();
private:
	sf::Text fps_text;
	Game &game;
};