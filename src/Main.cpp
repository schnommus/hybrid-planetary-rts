#include "CameraSystem.h"
#include "Components.h"
#include "DebugSystems.h"
#include "MinimapSystem.h"
#include "ProcessingSystem.h"
#include "RenderSystem.h"
#include <../gamemath/vector3.h>

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
void PlaceRandom(artemis::EntityManager* em, int n, std::string type, bool (*restrict)(float , float) = NoRestrict, bool isNode = false, int frames = 1 ) {
	for(int i = 0; i != n; ++i) {
		float u = float(rand()%1000)/1000.0f;
		float v = float(rand()%1000)/1000.0f;

		if( (*restrict)(u, v) == true ) {
			artemis::Entity & ent = em->create();
			ent.addComponent(new UVPositionComponent(u, v));
			ent.addComponent(new SpriteComponent(type, 1.0f, frames));
			if( isNode ) {
				ent.addComponent( new TerrainNodeComponent( type ) );
			}
			ent.addComponent(new MinimapComponent());
			ent.refresh();
		} else {
			--i; // Failed to place
		}
	}
}

int main(int argc, char **argv) {
	// Artemis containers
    artemis::World world;
    artemis::SystemManager * sm = world.getSystemManager();
    artemis::EntityManager * em = world.getEntityManager();

	// SFML rendering devices
	sf::RenderWindow realwindow(sf::VideoMode(1280, 720), "GamePrototype", sf::Style::Default );
	sf::RenderTexture window; window.create(427, 240);

	// Create all systems
	CameraSystem *cameraSys =
		(CameraSystem*)sm->setSystem(new CameraSystem());

	BackgroundTerrainRenderSystem * terrainRenderSys =
		(BackgroundTerrainRenderSystem*)sm->setSystem(new BackgroundTerrainRenderSystem(window, *cameraSys));

	UVSphericalRenderSystem * uvRenderSys =
		(UVSphericalRenderSystem*)sm->setSystem(new UVSphericalRenderSystem(window, *cameraSys, *terrainRenderSys));

	StarSystem * starSys =
		(StarSystem*)sm->setSystem(new StarSystem(window, *cameraSys));

	MinimapSphericalRenderSystem *minimapRenderSys =
		(MinimapSphericalRenderSystem*)sm->setSystem(new MinimapSphericalRenderSystem(window, *cameraSys));

	DrawFPSSystem * fpsSys =
		(DrawFPSSystem*)sm->setSystem(new DrawFPSSystem(window));

    sm->initializeAll();

    PlaceRandom(em, 170, "Desert1.png", AvoidPolarRestrict, true);
	PlaceRandom(em, 150, "Desert2.png", AvoidPolarRestrict, true);
	PlaceRandom(em, 35, "Desert3.png", AvoidPolarRestrict, true);

	PlaceRandom(em, 20, "Snow1.png", PolarRestrict, true);
	PlaceRandom(em, 20, "Snow2.png", PolarRestrict, true);

	PlaceRandom(em, 40, "Rock1.png");
	PlaceRandom(em, 50, "AirDispensor.png", NoRestrict, false, 25);
	PlaceRandom(em, 5, "DroneDown.png", NoRestrict, false, 3);
	PlaceRandom(em, 5, "DroneLeft.png", NoRestrict, false, 3);
	PlaceRandom(em, 5, "DroneRight.png", NoRestrict, false, 3);
	PlaceRandom(em, 5, "DroneUp.png", NoRestrict, false, 3);
	PlaceRandom(em, 10, "Artefact1.png");
	PlaceRandom(em, 40, "Plant1.png", AvoidPolarRestrict);
	PlaceRandom(em, 40, "Plant2.png", AvoidPolarRestrict);

	// Add all the background terrain entities
	for(int i = 0; i != (int)(ceil(window.getSize().x/16.0f)+2); ++i) {
		for(int j = 0; j != (int)(ceil(window.getSize().y/16.0f)+2); ++j) {
			artemis::Entity & e = em->create();
			e.addComponent(new FlatPositionComponent(16*i-16, 16*j-16));
			e.addComponent(new SpriteComponent("Desert1.png", 1.005f));
			e.addComponent(new BackgroundTerrainComponent());
			e.refresh();
		}
	}

	sf::Clock clock;

	sf::Texture uitex; uitex.loadFromFile("../media/uiOverlay.png");
	sf::Sprite uispr(uitex);
	uispr.setScale(1, 1);
	uispr.setPosition(window.getSize().x-109, window.getSize().y-63);

	sf::Sprite pixelrenderer;
	pixelrenderer.setScale(3, 3);

	bool firstFrame = true;
	
	std::vector<std::string> nodeTypes;
	nodeTypes.push_back("Desert1.png");
	nodeTypes.push_back("Desert2.png");
	nodeTypes.push_back("Desert3.png");
	nodeTypes.push_back("Snow1.png");
	nodeTypes.push_back("Snow2.png");

	int nodeTypeIndex = 0;

	while (realwindow.isOpen()) {
		sf::Event event;
		while (realwindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				realwindow.close();
		}

		//logic
		world.loopStart();
		world.setDelta( clock.restart().asSeconds() );
		

		window.clear(sf::Color(66, 55, 42));

		cameraSys->process();

		// drawing
		if( firstFrame ) { // This is so that terrain nodes exist before initial spriting action
			firstFrame = false;
			uvRenderSys->process();
		}
		terrainRenderSys->process();
		uvRenderSys->process();
		minimapRenderSys->process();

		window.draw(uispr);
		starSys->process();
		fpsSys->process();

		window.display();

		pixelrenderer.setTexture( window.getTexture() );
		realwindow.draw(pixelrenderer);
		realwindow.display();


		if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			artemis::Entity & ent = em->create();
			sf::Vector2i mpos = sf::Mouse::getPosition(realwindow);
			mpos.x /=3; mpos.y /=3;
			mpos.x -= window.getSize().x/2;
			mpos.y -= window.getSize().y/2;
			float sz = 500.0f;
			float z = sqrt( sz*sz-mpos.x*mpos.x-mpos.y*mpos.y );
			sf::Vector2f out = ReverseUVTransform( Vector3( mpos.x, mpos.y, z ), sz, cameraSys->worldtransform);
			ent.addComponent(new UVPositionComponent(out.x, out.y));
			ent.addComponent(new SpriteComponent(nodeTypes[nodeTypeIndex], 1.0f));
			ent.addComponent( new TerrainNodeComponent( nodeTypes[nodeTypeIndex] ) );
			ent.addComponent(new MinimapComponent());
			ent.refresh();
			while( sf::Mouse::isButtonPressed(sf::Mouse::Left) );
			terrainRenderSys->initialize();
			firstFrame = true;
		}

		if( sf::Mouse::isButtonPressed(sf::Mouse::Right) ) {
			++nodeTypeIndex;
			if( nodeTypeIndex == nodeTypes.size() ) nodeTypeIndex = 0;
			while( sf::Mouse::isButtonPressed(sf::Mouse::Right) );
		}

	}

    return 0;
}
