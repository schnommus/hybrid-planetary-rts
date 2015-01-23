#include "LevelEditor.h"

#include "EntityFactory.h"
#include <../gamemath/vector3.h>

LevelEditorSystem::LevelEditorSystem( sf::RenderTarget &windowv, sf::RenderWindow &realWindowv, CameraSystem *cameraSysv, BackgroundTerrainRenderSystem *terrainRenderSysv, UVSphericalRenderSystem *uvRenderSysv, EntityFactory *entFactoryv )
	: window( windowv ), realWindow(realWindowv), cameraSys(cameraSysv), terrainRenderSys(terrainRenderSysv), uvRenderSys(uvRenderSysv), entFactory(entFactoryv) { }

void LevelEditorSystem::initialize() {
	for( int i = 0; i != entFactory->GetAllTypes().size(); ++i ) {
		artemis::Entity &ent = *entFactory->Create(entFactory->GetAllTypes()[i]);
		sprites.push_back( FetchComponent<SpriteComponent>(ent).sprite );
		//ent.remove(); //TODO: after resource fetcher is built in; textures pointers need not be invalidated
	}

	typeIndex = 0;
	terrainAltered = false;
	debugfont.loadFromFile("..//media//RiskofRainFont.ttf");
	topInstructions.setFont(debugfont);
	topInstructions.setCharacterSize(7);
	topInstructions.setString("Place: LMB. Cycle: N/M. Delete: RMB. Selector: ");
}

bool LevelEditorSystem::queryTerrainAlterations() {
	if( terrainAltered )  {
		terrainAltered = false;
		return true;
	}
	return false;
}

void LevelEditorSystem::entitySelector() {
	// Draw the entity selector
	for( int k = 0,j = 0,i = 0; i != entFactory->GetAllTypes().size(); ++i, ++k ) {
		if( i % 5 == 0 ) ++j, k=0;
		sf::Sprite &sprite = sprites[i];
		sprite.setPosition(290+k*18, 10+18*(j-1));
		if( i == typeIndex ) sprite.setScale(1.3, 1.3);
		else sprite.setScale(1, 1);
		window.draw( sprite );
	}
	topInstructions.setPosition( 50, 13 );
	window.draw( topInstructions );
	
	// Use keys to change selected
	if( sf::Keyboard::isKeyPressed(sf::Keyboard::M) ) {
		++typeIndex;
		if( typeIndex ==entFactory->GetAllTypes().size() ) typeIndex = 0;
		while( sf::Keyboard::isKeyPressed(sf::Keyboard::M) );
	}

	if( sf::Keyboard::isKeyPressed(sf::Keyboard::N) ) {
		--typeIndex;
		if( typeIndex == -1 ) typeIndex = entFactory->GetAllTypes().size()-1;
		while( sf::Keyboard::isKeyPressed(sf::Keyboard::N) );
	}
}

void LevelEditorSystem::placeEntities() {
	if( sf::Mouse::isButtonPressed( sf::Mouse::Left ) ) {
		sf::Vector2i mpos = sf::Mouse::getPosition(realWindow);
		mpos.x /= 3; // In reality screen is 3x bigger than 'pixelspace'
		mpos.y /= 3;
		mpos.x -= window.getSize().x/2;
		mpos.y -= window.getSize().y/2;
		float sz = 500.0f;
		float z = sqrt( sz*sz-mpos.x*mpos.x-mpos.y*mpos.y );
		sf::Vector2f out = ReverseUVTransform( Vector3( mpos.x, mpos.y, z ), sz, cameraSys->worldtransform);
		artemis::Entity & ent = *entFactory->Create(entFactory->GetAllTypes()[typeIndex]);
		FetchComponent<UVPositionComponent>(ent).u = out.x;
		FetchComponent<UVPositionComponent>(ent).v = out.y;
		while( sf::Mouse::isButtonPressed( sf::Mouse::Left ) );
		
		terrainAltered = true;
	}
}

void LevelEditorSystem::removeEntities() {
	if( sf::Mouse::isButtonPressed( sf::Mouse::Right ) ) {
		sf::Vector2i mpos = sf::Mouse::getPosition(realWindow);
		mpos.x /= 3; // In reality screen is 3x bigger than 'pixelspace'
		mpos.y /= 3;
		
		for( int i = 0; i != world->getEntityManager()->getTotalCreated(); ++i ) {
			if( &world->getEntityManager()->getEntity(i) != nullptr ) {
				if( world->getEntityManager()->getEntity(i).getComponent<UVPositionComponent>() != nullptr ) {
					UVPositionComponent &pos = *((UVPositionComponent*)world->getEntityManager()->getEntity(i).getComponent<UVPositionComponent>());
					if( pos.on_screen ) {
						if( sqrt( (pos.screen_x-mpos.x)*(pos.screen_x-mpos.x) + (pos.screen_y-mpos.y)*(pos.screen_y-mpos.y) ) < 10 ){
							world->getEntityManager()->remove( world->getEntityManager()->getEntity(i));
						}
					}
				}
			}
		}
		
		//Uncomment to allow deletion of only one object at a time
		//while( sf::Mouse::isButtonPressed( sf::Mouse::Right ) );

		terrainAltered = true;
	}
}

void LevelEditorSystem::doProcessing() {
	entitySelector();

	placeEntities();

	removeEntities();
}