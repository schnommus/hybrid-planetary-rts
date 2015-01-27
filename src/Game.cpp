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
#include "SelectionSystem.h"

#include <fstream>
#include <sstream>

void Game::Initialize() {
	// Artemis containers
	artemis::SystemManager * sm = world.getSystemManager();
	
	// SFML rendering devices
	realwindow = new sf::RenderWindow(sf::VideoMode(1280, 720), "GamePrototype", sf::Style::Default );
	window = new sf::RenderTexture();
	window->create(427, 240);

	// Create all systems
	cameraSys =
		(CameraSystem*)sm->setSystem(new CameraSystem(*this));

	terrainRenderSys =
		(BackgroundTerrainRenderSystem*)sm->setSystem(new BackgroundTerrainRenderSystem(*this));

	uvRenderSys =
		(UVSphericalRenderSystem*)sm->setSystem(new UVSphericalRenderSystem(*this, *terrainRenderSys));

	minimapRenderSys =
		(MinimapSphericalRenderSystem*)sm->setSystem(new MinimapSphericalRenderSystem(*this));

	fpsSys =
		(DrawDebugInfoSystem*)sm->setSystem(new DrawDebugInfoSystem(*this));

	entFactory = 
		(XMLEntityFactory*)sm->setSystem( new XMLEntityFactory() );

	uiSys = 
		(UIRenderSystem*)sm->setSystem( new UIRenderSystem(*this));

	levelEditorSys =
		(LevelEditorSystem*)sm->setSystem(new LevelEditorSystem(*this, *realwindow, terrainRenderSys, uvRenderSys));

	selectionSys = 
		(SelectionSystem*)sm->setSystem(new SelectionSystem(*this));

	sm->initializeAll();
}

void Game::Run() {
	entFactory->Create("leftpanel");

	sf::Clock clock;

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

		selectionSys->process();
		selectionSys->drawUnder();

		uvRenderSys->process();

		minimapRenderSys->process();
		minimapRenderSys->drawStars();

		uiSys->process();

		selectionSys->drawOver();

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

void Game::GenerateLevel() {
	PlaceRandom(entFactory, 130, "terrain_desert1", AvoidPolarRestrict);
	PlaceRandom(entFactory, 130, "terrain_desert2", AvoidPolarRestrict);
	PlaceRandom(entFactory, 55, "terrain_desert3", AvoidPolarRestrict);

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
	terrainRenderSys->Recalculate();
	terrainRenderSys->process();
}

void Game::SaveLevel() {
	std::ofstream file;
	file.open( "../media/level.txt" );
	for( int i = 0; i != world.getEntityManager()->getTotalCreated(); ++i ) {
		if( &world.getEntityManager()->getEntity(i) != nullptr ) {
			if( world.getEntityManager()->getEntity(i).getComponent<UVPositionComponent>() != nullptr ) {
				UVPositionComponent &pos = *((UVPositionComponent*)world.getEntityManager()->getEntity(i).getComponent<UVPositionComponent>());
				file << FetchComponent<NameComponent>(world.getEntityManager()->getEntity(i)).name << " " << pos.u << " " << pos.v << std::endl;
			}
		}
	}
	file.close();
}

void Game::LoadLevel() {
	std::string line;
	std::ifstream file ("../media/level.txt");
	if (file.is_open()) {
		while ( getline (file,line) ) {
			std::istringstream iss(line);
			std::string type;
			float u, v;
			iss >> type >> u >> v;
			artemis::Entity & ent = *EntityFactory()->Create(type);
			FetchComponent<UVPositionComponent>(ent).u = u;
			FetchComponent<UVPositionComponent>(ent).v = v;
		}
		file.close();
	}

	else std::cout << "Unable to level file"; 
}

