#pragma once

#include "ProcessingSystem.h"
#include "Game.h"
#include <SFML/Graphics.hpp>

class DrawFPSSystem : public artemis::ProcessingSystem {
public:
	DrawFPSSystem (Game &gamev);
protected:
	virtual void doProcessing ();
	virtual void initialize ();
private:
	sf::Text fps_text;
	Game &game;
};