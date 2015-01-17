#pragma once

#include "Components.h"
#include "CameraSystem.h"
#include <Artemis/Artemis.h>
#include <SFML/Graphics.hpp>

class StarSystem : public artemis::EntityProcessingSystem {
	artemis::ComponentMapper <FlatPositionComponent> positionMapper;
	sf::Texture starTex;
	sf::Sprite starSprite;
	sf::RenderTarget & window;
	CameraSystem & cameraSystem;
public:
	StarSystem (sf::RenderTarget & rwindow, CameraSystem & cameraSystemv);
	virtual void initialize ();
	virtual void processEntity (artemis::Entity & e);
};

class MinimapSphericalRenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper <UVPositionComponent> positionMapper;
	artemis::ComponentMapper <SpriteComponent> spriteMapper;
	artemis::ComponentMapper <MinimapComponent> minimapMapper;
	sf::RenderTarget & window;
	CameraSystem & cameraSystem;
	float sz;
public:
	MinimapSphericalRenderSystem (sf::RenderTarget & rwindow, CameraSystem & cameraSystemv);
	virtual void initialize ();
	virtual void processEntity (artemis::Entity & e);
};