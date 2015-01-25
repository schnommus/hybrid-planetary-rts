#include "LevelEditor.h"

#include "EntityFactory.h"
#include <../gamemath/vector3.h>
#include "ResourceManager.h"

LevelEditorSystem::LevelEditorSystem( Game &gamev, sf::RenderWindow &realWindowv, BackgroundTerrainRenderSystem *terrainRenderSysv, UVSphericalRenderSystem *uvRenderSysv )
	: game(gamev), realWindow(realWindowv), terrainRenderSys(terrainRenderSysv), uvRenderSys(uvRenderSysv) { }

void LevelEditorSystem::initialize() {
	for( int i = 0; i != game.EntityFactory()->GetAllTypes().size(); ++i ) {
		artemis::Entity &ent = *game.EntityFactory()->Create(game.EntityFactory()->GetAllTypes()[i]);
		sprites.push_back( FetchComponent<SpriteComponent>(ent).sprite );
		ent.remove();
	}

	typeIndex = 0;
	terrainAltered = false;
	topInstructions.setFont( ResourceManager::Inst().GetFont("RiskofRainFont.ttf") );
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
	for( int k = 0,j = 0,i = 0; i != game.EntityFactory()->GetAllTypes().size(); ++i, ++k ) {
		if( i % 5 == 0 ) ++j, k=0;
		sf::Sprite &sprite = sprites[i];
		sprite.setPosition(290+k*18, 10+18*(j-1));
		float scale = 16.0f / (float)sprite.getLocalBounds().width; //Normalize to 16x16
		if( i == typeIndex ) sprite.setScale(1.3*scale, 1.3*scale);
		else sprite.setScale(scale, scale);
		game.Renderer()->draw( sprite );
	}
	topInstructions.setPosition( 50, 13 );
	game.Renderer()->draw( topInstructions );
	
	// Use keys to change selected
	if( sf::Keyboard::isKeyPressed(sf::Keyboard::M) ) {
		++typeIndex;
		if( typeIndex == game.EntityFactory()->GetAllTypes().size() ) typeIndex = 0;
		while( sf::Keyboard::isKeyPressed(sf::Keyboard::M) );
	}

	if( sf::Keyboard::isKeyPressed(sf::Keyboard::N) ) {
		--typeIndex;
		if( typeIndex == -1 ) typeIndex = game.EntityFactory()->GetAllTypes().size()-1;
		while( sf::Keyboard::isKeyPressed(sf::Keyboard::N) );
	}
}

void LevelEditorSystem::placeEntities() {
	if( sf::Mouse::isButtonPressed( sf::Mouse::Left ) ) {
		sf::Vector2i mpos = sf::Mouse::getPosition(realWindow);
		mpos.x /= 3; // In reality screen is 3x bigger than 'pixelspace'
		mpos.y /= 3;
		mpos.x -= game.Renderer()->getSize().x/2;
		mpos.y -= game.Renderer()->getSize().y/2;
		float sz = 500.0f;
		float z = sqrt( sz*sz-mpos.x*mpos.x-mpos.y*mpos.y );
		sf::Vector2f out = ReverseUVTransform( Vector3( mpos.x, mpos.y, z ), sz, game.Camera()->worldtransform);
		artemis::Entity & ent = *game.EntityFactory()->Create(game.EntityFactory()->GetAllTypes()[typeIndex]);
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