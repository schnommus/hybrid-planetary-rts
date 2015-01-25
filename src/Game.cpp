#include "Game.h"

#include "CameraSystem.h"
#include "Components.h"
#include "DebugSystems.h"
#include "MinimapSystem.h"
#include "ProcessingSystem.h"
#include "RenderSystem.h"
#include "LevelEditor.h"
#include "EntityFactory.h"
#include "ResourceManager.h"

void Game::Initialize() {
	// Artemis containers
	artemis::SystemManager * sm = world.getSystemManager();
	
	// SFML rendering devices
	realwindow = new sf::RenderWindow(sf::VideoMode(1280, 720), "GamePrototype", sf::Style::Default );
	window = new sf::RenderTexture();
	window->create(427, 240);

	// Create all systems
	cameraSys =
		(CameraSystem*)sm->setSystem(new CameraSystem());

	terrainRenderSys =
		(BackgroundTerrainRenderSystem*)sm->setSystem(new BackgroundTerrainRenderSystem(*this));

	uvRenderSys =
		(UVSphericalRenderSystem*)sm->setSystem(new UVSphericalRenderSystem(*this, *terrainRenderSys));

	minimapRenderSys =
		(MinimapSphericalRenderSystem*)sm->setSystem(new MinimapSphericalRenderSystem(*this));

	fpsSys =
		(DrawFPSSystem*)sm->setSystem(new DrawFPSSystem(*this));

	entFactory = 
		(XMLEntityFactory*)sm->setSystem( new XMLEntityFactory() );

	levelEditorSys =
		(LevelEditorSystem*)sm->setSystem(new LevelEditorSystem(*this, *realwindow, terrainRenderSys, uvRenderSys));

	sm->initializeAll();
}

void Game::Run() {
	sf::Clock clock;

	sf::Sprite uispr( ResourceManager::Inst().GetTexture("uiOverlay.png") );
	uispr.setScale(1, 1);
	uispr.setPosition(Renderer()->getSize().x-109, Renderer()->getSize().y-63);

	sf::Sprite pixelrenderer;
	pixelrenderer.setScale(3, 3);

	while (realwindow->isOpen()) {
		sf::Event event;
		while (realwindow->pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				realwindow->close();
		}

		//logic
		world.loopStart();
		world.setDelta( clock.restart().asSeconds() );


		Renderer()->clear(sf::Color(66, 55, 42));

		cameraSys->process();

		terrainRenderSys->process();
		uvRenderSys->process();
		minimapRenderSys->process();

		Renderer()->draw(uispr);
		minimapRenderSys->drawStars();
		fpsSys->process();

		if( levelEditorEnabled ) {
			levelEditorSys->process();
		}

		Renderer()->display();

		pixelrenderer.setTexture( Renderer()->getTexture() );
		realwindow->draw(pixelrenderer);
		realwindow->display();

		if( queueTerrainRecalculation ) {
			DoTerrainRecalculation();
			queueTerrainRecalculation = false;
		}
	}
}

bool NoRestrict(float u, float v) {
	return true;
}

bool PolarRestrict(float u, float v) {
	return v < 0.1 || (v>0.4 && v<0.6) || v > 0.9;
}

bool AvoidPolarRestrict(float u, float v) {
	return (v > 0.1 && v < 0.4) || (v > 0.6 && v < 0.9);
}

// A restrict function will return true if the goal position is valid
void PlaceRandom(XMLEntityFactory* ef, int n, std::string type, bool (*restrict)(float , float) = NoRestrict ) {
	for(int i = 0; i != n; ++i) {
		float u = float(rand()%1000)/1000.0f;
		float v = float(rand()%1000)/1000.0f;

		if( (*restrict)(u, v) == true ) {
			artemis::Entity & ent = *ef->Create(type);
			FetchComponent<UVPositionComponent>(ent).u = u;
			FetchComponent<UVPositionComponent>(ent).v = v;
		} else {
			--i; // Failed to place
		}
	}
}

void Game::LoadLevel() {
	PlaceRandom(entFactory, 130, "terrain_desert1", AvoidPolarRestrict);
	PlaceRandom(entFactory, 130, "terrain_desert2", AvoidPolarRestrict);
	PlaceRandom(entFactory, 55, "terrain_desert3", AvoidPolarRestrict);

	PlaceRandom(entFactory, 50, "airdispensor", NoRestrict );
	PlaceRandom(entFactory, 5, "artefact", NoRestrict );
	PlaceRandom(entFactory, 40, "plant1", AvoidPolarRestrict );
	PlaceRandom(entFactory, 40, "plant2", AvoidPolarRestrict );
	PlaceRandom(entFactory, 40, "rock1", PolarRestrict );

	PlaceRandom(entFactory, 20, "terrain_snow1", PolarRestrict);
	PlaceRandom(entFactory, 20, "terrain_snow2", PolarRestrict);
}

void Game::EnableLevelEditor() {
	levelEditorEnabled = true;
}

Game::Game() : levelEditorEnabled(false), queueTerrainRecalculation(true) {

}

sf::Vector2i Game::LocalMousePosition() {
	sf::Vector2i mpos = sf::Mouse::getPosition(*realwindow);
	mpos.x /= 3; // In reality screen is 3x bigger than 'pixelspace'
	mpos.y /= 3;
	return mpos;
}

void Game::RecalculateTerrain() {
	queueTerrainRecalculation = true;
}

void Game::DoTerrainRecalculation() {
	world.loopStart();
	cameraSys->process();
	uvRenderSys->process();
	terrainRenderSys->reSpriteAll();
	terrainRenderSys->process();
}

