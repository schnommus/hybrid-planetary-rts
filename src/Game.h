#pragma once

#include <Artemis/Artemis.h>
#include <SFML/Graphics.hpp>

class CameraSystem;
class XMLEntityFactory;
class BackgroundTerrainRenderSystem;
class UVSphericalRenderSystem;
class LevelEditorSystem;
class MinimapSphericalRenderSystem;
class DrawDebugInfoSystem;
class UIRenderSystem;

class Game {
public:
	Game();
	void Initialize();
	void LoadLevel();
	void Run();

	void RecalculateTerrain();

	void EnableLevelEditor();
	bool LevelEditorEnabled() const { return levelEditorEnabled; }

	CameraSystem * Camera() { return cameraSys; }
	XMLEntityFactory * EntityFactory() { return entFactory; }
	sf::RenderTexture * Renderer() { return window; }
	
	sf::Vector2i LocalMousePosition();

private:
	void DoTerrainRecalculation();

	artemis::World world;
	bool levelEditorEnabled;
	bool queueTerrainRecalculation;

	CameraSystem *cameraSys;
	XMLEntityFactory *entFactory;

	sf::RenderTexture *window;
	sf::RenderWindow *realwindow;

	// These systems don't have accessors as no-one should really need to get at them
	BackgroundTerrainRenderSystem *terrainRenderSys;
	UVSphericalRenderSystem *uvRenderSys;
	LevelEditorSystem *levelEditorSys;
	MinimapSphericalRenderSystem *minimapRenderSys;
	DrawDebugInfoSystem *fpsSys;
	UIRenderSystem *uiSys;
};