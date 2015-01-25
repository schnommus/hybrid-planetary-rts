#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <Artemis/Artemis.h>

#include "ProcessingSystem.h"
#include "CameraSystem.h"
#include "RenderSystem.h"

class XMLEntityFactory;

class LevelEditorSystem : public artemis::ProcessingSystem {
public:
	LevelEditorSystem( Game &gamev, sf::RenderWindow &realwindowv, BackgroundTerrainRenderSystem *terrainRenderSysv, UVSphericalRenderSystem *uvRenderSysv );
	bool queryTerrainAlterations();
protected:
	virtual void initialize();
	virtual void doProcessing();
	
	void placeEntities();
	void removeEntities();
	void entitySelector();
private:
	Game &game;
	sf::RenderWindow &realWindow;
	BackgroundTerrainRenderSystem *terrainRenderSys;
	UVSphericalRenderSystem *uvRenderSys;

	std::vector< sf::Sprite > sprites;
	int typeIndex;
	bool terrainAltered;
	sf::Text topInstructions;
	sf::Font debugfont;
};
