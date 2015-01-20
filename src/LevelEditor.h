#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <Artemis/Artemis.h>

#include "ProcessingSystem.h"
#include "CameraSystem.h"
#include "RenderSystem.h"

class LevelEditorSystem : public artemis::ProcessingSystem {
public:
	LevelEditorSystem( sf::RenderTarget &windowv, sf::RenderWindow &realwindowv, CameraSystem *cameraSysv, BackgroundTerrainRenderSystem *terrainRenderSysv, UVSphericalRenderSystem *uvRenderSysv );
	bool queryTerrainAlterations();
protected:
	virtual void initialize();
	virtual void doProcessing();

	void populateTypes();
	void addType(std::string type);
private:
	sf::RenderTarget &window;	
	sf::RenderWindow &realWindow;
	BackgroundTerrainRenderSystem *terrainRenderSys;
	CameraSystem *cameraSys;
	UVSphericalRenderSystem *uvRenderSys;

	std::vector<std::string> types;
	int typeIndex;
	bool terrainAltered;
};
