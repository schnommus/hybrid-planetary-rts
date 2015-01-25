#pragma once

#include <Artemis/Artemis.h>
#include <SFML/Graphics.hpp>
#include <../gamemath/Matrix4x3.h>
#include "Components.h"
#include "Game.h"


Vector3 DoUVTransform (float u, float v, float sz, Matrix4x3 & world);
sf::Vector2f ReverseUVTransform (Vector3 in, float sz, Matrix4x3 & world);

class BackgroundTerrainRenderSystem : public artemis::EntityProcessingSystem
{
private:

	artemis::ComponentMapper <FlatPositionComponent> positionMapper;
	artemis::ComponentMapper <SpriteComponent> spriteMapper;
	std::vector <int> nodeIds;
	bool initialized;
	Game &game;
protected:

	virtual void initialize ();
	virtual void processEntities (artemis::ImmutableBag <artemis::Entity*> & bag);
	void reSprite (artemis::Entity & e);
	virtual void processEntity (artemis::Entity & e);

public:
	void addNodeID (int id);
	void clearNodeIDs ();
	BackgroundTerrainRenderSystem (Game &gamev);
	void Recalculate();
};

class UIRenderSystem : public artemis::EntityProcessingSystem
{
private:
	artemis::ComponentMapper <FlatPositionComponent> positionMapper;
	artemis::ComponentMapper <SpriteComponent> spriteMapper;
	Game &game;
protected:
	virtual void initialize ();
	virtual void processEntity (artemis::Entity & e);
public:
	UIRenderSystem(Game &gamev);
};



class UVSphericalRenderSystem : public artemis::EntityProcessingSystem
{
private:
	artemis::ComponentMapper <UVPositionComponent> positionMapper;
	artemis::ComponentMapper <SpriteComponent> spriteMapper;
	Game &game;
	BackgroundTerrainRenderSystem & flatSystem;
public:
	UVSphericalRenderSystem (Game &gamev, BackgroundTerrainRenderSystem & flatSystemv);
	virtual void initialize ();
	virtual void processEntities (artemis::ImmutableBag <artemis::Entity*> & bag);
	virtual void processEntity (artemis::Entity & e);
};
