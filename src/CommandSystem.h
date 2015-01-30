#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <Artemis/Artemis.h>

#include "Game.h"
#include "ProcessingSystem.h"

class CommandSystem : public artemis::ProcessingSystem {
public:
	CommandSystem(Game &gamev);
protected:
	virtual void initialize();
	virtual void doProcessing();
	void PerformAction();
private:
	Game &game;
	bool rmbDown;
};
