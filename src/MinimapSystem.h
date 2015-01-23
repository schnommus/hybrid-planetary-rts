#pragma once

#include "Components.h"
#include "CameraSystem.h"
#include <Artemis/Artemis.h>
#include <SFML/Graphics.hpp>

class StarSystem : public artemis::EntityProcessingSystem {
	artemis::ComponentMapper <FlatPositionComponent> positionMapper;
	std::vector<sf::Vector2f> starPositions;
	sf::RenderTarget & window;
	CameraSystem & cameraSystem;
public:
	StarSystem (sf::RenderTarget & rwindow, CameraSystem & cameraSystemv);
	virtual void initialize ();
	virtual void processEntity (artemis::Entity & e);
	virtual void processEntities (artemis::ImmutableBag <artemis::Entity*> & bag);
};

class MinimapSphericalRenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper <UVPositionComponent> positionMapper;
	artemis::ComponentMapper <SpriteComponent> spriteMapper;
	artemis::ComponentMapper <MinimapComponent> minimapMapper;
	sf::RenderTarget & window;
	CameraSystem & cameraSystem;
	StarSystem * starSys;
	float sz;
public:
	MinimapSphericalRenderSystem (sf::RenderTarget & rwindow, CameraSystem & cameraSystemv);
	void drawStars();
	virtual void initialize ();
	virtual void processEntity (artemis::Entity & e);
};
