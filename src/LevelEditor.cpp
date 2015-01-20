#include "LevelEditor.h"
#include <../gamemath/vector3.h>

LevelEditorSystem::LevelEditorSystem( sf::RenderTarget &windowv, sf::RenderWindow &realWindowv, CameraSystem *cameraSysv, BackgroundTerrainRenderSystem *terrainRenderSysv, UVSphericalRenderSystem *uvRenderSysv )
	: window( windowv ), realWindow(realWindowv), cameraSys(cameraSysv), terrainRenderSys(terrainRenderSysv), uvRenderSys(uvRenderSysv) { }

void LevelEditorSystem::initialize() {
	populateTypes();
	typeIndex = 0;
	terrainAltered = false;
}

bool LevelEditorSystem::queryTerrainAlterations() {
	if( terrainAltered ) return true;
	terrainAltered = false;
	return false;
}

void LevelEditorSystem::doProcessing() {
	if( sf::Mouse::isButtonPressed( sf::Mouse::Left ) ) {
		artemis::Entity &ent = world->createEntity();
		sf::Vector2i mpos = sf::Mouse::getPosition(realWindow);
		mpos.x /= 3; // In reality screen is 3x bigger than 'pixelspace'
		mpos.y /= 3;
		mpos.x -= window.getSize().x/2;
		mpos.y -= window.getSize().y/2;
		float sz = 500.0f;
		float z = sqrt( sz*sz-mpos.x*mpos.x-mpos.y*mpos.y );
		sf::Vector2f out = ReverseUVTransform( Vector3( mpos.x, mpos.y, z ), sz, cameraSys->worldtransform);
		ent.addComponent( new UVPositionComponent( out.x, out.y ));
		ent.addComponent( new SpriteComponent( types[typeIndex] ) );
		ent.addComponent( new TerrainNodeComponent( types[typeIndex] ) );
		ent.addComponent( new MinimapComponent() );
		ent.refresh();

		while( sf::Mouse::isButtonPressed( sf::Mouse::Left ) );
		
		terrainAltered = true;
		//TODO: Refactor whole firstframe business
		//Remove initialization counters etc
	}	

	if( sf::Mouse::isButtonPressed( sf::Mouse::Right ) ) {
		++typeIndex;
		if( typeIndex == types.size() ) typeIndex = 0;
		while( sf::Mouse::isButtonPressed(sf::Mouse::Right) );
	}
}


void LevelEditorSystem::addType( std::string type ) {
	types.push_back(type);
}

void LevelEditorSystem::populateTypes() {
	addType("Desert1.png");
	addType("Desert2.png");
	addType("Desert3.png");
	addType("Snow1.png");
	addType("Snow2.png");
}
