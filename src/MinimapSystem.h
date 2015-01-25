#pragma once

#include "Components.h"
#include "Game.h"
#include <Artemis/Artemis.h>
#include <SFML/Graphics.hpp>

class StarSystem : public artemis::EntityProcessingSystem {
	artemis::ComponentMapper <FlatPositionComponent> positionMapper;
	std::vector<sf::Vector2f> starPositions;
	Game &game;
public:
	StarSystem (Game &gamev);
	virtual void initialize ();
	virtual void processEntity (artemis::Entity & e);
	virtual void processEntities (artemis::ImmutableBag <artemis::Entity*> & bag);
};

class MinimapSphericalRenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper <UVPositionComponent> positionMapper;
	artemis::ComponentMapper <SpriteComponent> spriteMapper;
	artemis::ComponentMapper <MinimapTag> minimapMapper;
	Game &game;
	StarSystem * starSys;
	float sz;
public:
	MinimapSphericalRenderSystem (Game &gamev);
	void drawStars();
	virtual void initialize ();
	virtual void processEntity (artemis::Entity & e);
};
