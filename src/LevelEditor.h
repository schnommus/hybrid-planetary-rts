#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <Artemis/Artemis.h>

#include "ProcessingSystem.h"
#include "CameraSystem.h"
#include "RenderSystem.h"

class EntityFactory;

class LevelEditorSystem : public artemis::ProcessingSystem {
public:
	LevelEditorSystem( sf::RenderTarget &windowv, sf::RenderWindow &realwindowv, CameraSystem *cameraSysv, BackgroundTerrainRenderSystem *terrainRenderSysv, UVSphericalRenderSystem *uvRenderSysv, EntityFactory *entFactoryv );
	bool queryTerrainAlterations();
protected:
	virtual void initialize();
	virtual void doProcessing();
	
	void placeEntities();
	void removeEntities();
	void entitySelector();
private:
	sf::RenderTarget &window;	
	sf::RenderWindow &realWindow;
	BackgroundTerrainRenderSystem *terrainRenderSys;
	CameraSystem *cameraSys;
	UVSphericalRenderSystem *uvRenderSys;
	EntityFactory *entFactory;

	std::vector< sf::Sprite > sprites;
	int typeIndex;
	bool terrainAltered;
	sf::Text topInstructions;
	sf::Font debugfont;
};
