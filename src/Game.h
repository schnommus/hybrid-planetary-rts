#pragma once

#include <Artemis/Artemis.h>
#include <SFML/Graphics.hpp>

class CameraSystem;
class XMLEntityFactory;
class BackgroundTerrainRenderSystem;
class UVSphericalRenderSystem;
class LevelEditorSystem;
class MinimapSphericalRenderSystem;
class DrawFPSSystem;

class Game {
public:
	Game();
	void Initialize();
	void LoadLevel();
	void Run();

	void EnableLevelEditor();
	bool LevelEditorEnabled() const { return levelEditorEnabled; }

	CameraSystem * Camera() { return cameraSys; }
	XMLEntityFactory * EntityFactory() { return entFactory; }
	sf::RenderTexture * Renderer() { return window; }

private:
	artemis::World world;
	bool levelEditorEnabled;

	CameraSystem *cameraSys;
	XMLEntityFactory *entFactory;

	sf::RenderTexture *window;
	sf::RenderWindow *realwindow;

	// These systems don't have accessors as no-one should really need to get at them
	BackgroundTerrainRenderSystem *terrainRenderSys;
	UVSphericalRenderSystem *uvRenderSys;
	LevelEditorSystem *levelEditorSys;
	MinimapSphericalRenderSystem *minimapRenderSys;
	DrawFPSSystem *fpsSys;
};