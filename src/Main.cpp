#include "CameraSystem.h"
#include "Components.h"
#include "DebugSystems.h"
#include "MinimapSystem.h"
#include "ProcessingSystem.h"
#include "RenderSystem.h"
#include "LevelEditor.h"
#include "EntityFactory.h"
#include "ResourceManager.h"

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
void PlaceRandom(EntityFactory* ef, int n, std::string type, bool (*restrict)(float , float) = NoRestrict ) {
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

    MinimapSphericalRenderSystem *minimapRenderSys =
        (MinimapSphericalRenderSystem*)sm->setSystem(new MinimapSphericalRenderSystem(window, *cameraSys));

    DrawFPSSystem * fpsSys =
        (DrawFPSSystem*)sm->setSystem(new DrawFPSSystem(window));

	EntityFactory *entFactory = 
		(EntityFactory*)sm->setSystem( new EntityFactory(window) );

    LevelEditorSystem * levelEditorSys =
        (LevelEditorSystem*)sm->setSystem(new LevelEditorSystem(window, realwindow, cameraSys, terrainRenderSys, uvRenderSys, entFactory));

    sm->initializeAll();

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

    // Add all the background terrain entities
    for(int i = 0; i != (int)(ceil(window.getSize().x/16.0f)+2); ++i) {
        for(int j = 0; j != (int)(ceil(window.getSize().y/16.0f)+2); ++j) {
            artemis::Entity & e = em->create();
            e.addComponent(new FlatPositionComponent(16*i-16, 16*j-16));
            e.addComponent(new SpriteComponent("Desert1.png", 1.005f));
            e.addComponent(new BackgroundTerrainTag());
            e.refresh();
        }
    }

    sf::Clock clock;

    sf::Sprite uispr( ResourceManager::Inst().GetTexture("uiOverlay.png") );
    uispr.setScale(1, 1);
    uispr.setPosition(window.getSize().x-109, window.getSize().y-63);

    sf::Sprite pixelrenderer;
    pixelrenderer.setScale(3, 3);

    bool firstFrame = true;

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
        minimapRenderSys->drawStars();
        fpsSys->process();
        
        levelEditorSys->process();
        if( levelEditorSys->queryTerrainAlterations() == true ) {
            firstFrame = true;
            terrainRenderSys->initialize();
        }

        window.display();

        pixelrenderer.setTexture( window.getTexture() );
        realwindow.draw(pixelrenderer);
        realwindow.display();

    }

    return 0;
}
