#pragma once

#include <Artemis/Artemis.h>
#include <SFML/Graphics.hpp>
#include <../gamemath/Matrix4x3.h>
#include "Components.h"

class CameraSystem;

Vector3 DoUVTransform (float u, float v, float sz, Matrix4x3 & world);

class BackgroundTerrainRenderSystem : public artemis::EntityProcessingSystem
{
private:
	artemis::ComponentMapper <FlatPositionComponent> positionMapper;
	artemis::ComponentMapper <SpriteComponent> spriteMapper;
	sf::RenderTarget & window;
	CameraSystem & cameraSystem;
	std::vector <int> nodeIds;
	bool initialized;
public:
	void addNodeID (int id);
	void clearNodeIDs ();
	BackgroundTerrainRenderSystem (sf::RenderTarget & rwindow, CameraSystem & cameraSystemv);
	virtual void initialize ();
	virtual void processEntities (artemis::ImmutableBag <artemis::Entity*> & bag);
	void reSprite (artemis::Entity & e);
	virtual void processEntity (artemis::Entity & e);
};


class UVSphericalRenderSystem : public artemis::EntityProcessingSystem
{
private:
	artemis::ComponentMapper <UVPositionComponent> positionMapper;
	artemis::ComponentMapper <SpriteComponent> spriteMapper;
	sf::RenderTarget & window;
	CameraSystem & cameraSystem;
	BackgroundTerrainRenderSystem & flatSystem;
public:
	UVSphericalRenderSystem (sf::RenderTarget & rwindow, CameraSystem & cameraSystemv, BackgroundTerrainRenderSystem & flatSystemv);
	virtual void initialize ();
	virtual void processEntities (artemis::ImmutableBag <artemis::Entity*> & bag);
	virtual void processEntity (artemis::Entity & e);
};