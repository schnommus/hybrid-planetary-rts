#include "LevelEditor.h"
#include <../gamemath/vector3.h>

LevelEditorSystem::LevelEditorSystem( sf::RenderTarget &windowv, sf::RenderWindow &realWindowv, CameraSystem *cameraSysv, BackgroundTerrainRenderSystem *terrainRenderSysv, UVSphericalRenderSystem *uvRenderSysv )
	: window( windowv ), realWindow(realWindowv), cameraSys(cameraSysv), terrainRenderSys(terrainRenderSysv), uvRenderSys(uvRenderSysv) { }

void LevelEditorSystem::initialize() {
	populateTypes();
	typeIndex = 0;
	terrainAltered = false;
	debugfont.loadFromFile("..//media//RiskofRainFont.ttf");
	topInstructions.setFont(debugfont);
	topInstructions.setCharacterSize(7);
	topInstructions.setString("Place: LMB. Cycle: N/M. Delete: RMB. Selector: ");
	//TODO: Scroll to cycle; RMB deletes.
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
	for( int k = 0,j = 0,i = 0; i != textures.size(); ++i, ++k ) {
		if( i % 5 == 0 ) ++j, k=0;
		currentSprite.setPosition(290+k*18, 10+18*(j-1));
		currentSprite.setTexture( textures[i] );
		if( i == typeIndex ) currentSprite.setScale(1.3, 1.3);
		else currentSprite.setScale(1, 1);
		window.draw( currentSprite );
	}
	topInstructions.setPosition( 50, 13 );
	window.draw( topInstructions );
	
	// Use keys to change selected
	if( sf::Keyboard::isKeyPressed(sf::Keyboard::M) ) {
		++typeIndex;
		if( typeIndex == types.size() ) typeIndex = 0;
		while( sf::Keyboard::isKeyPressed(sf::Keyboard::M) );
	}

	if( sf::Keyboard::isKeyPressed(sf::Keyboard::N) ) {
		--typeIndex;
		if( typeIndex == -1 ) typeIndex = types.size()-1;
		while( sf::Keyboard::isKeyPressed(sf::Keyboard::N) );
	}
}

void LevelEditorSystem::placeEntities() {
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
		if( nodePredicates[typeIndex] )
			ent.addComponent( new TerrainNodeComponent( types[typeIndex] ) );
		ent.addComponent( new MinimapComponent() );
		ent.refresh();

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


void LevelEditorSystem::addType( std::string type, bool isNode ) {
	types.push_back(type);
	nodePredicates.push_back(isNode);
	textures.push_back( sf::Texture() );
	textures[textures.size()-1].loadFromFile( std::string("../media/") + type );
}

void LevelEditorSystem::populateTypes() {
	addType("Desert1.png", true);
	addType("Desert2.png", true);
	addType("Desert3.png", true);
	addType("Snow1.png", true);
	addType("Snow2.png", true);

	addType("Plant1.png");
	addType("Plant2.png");
	addType("Rock1.png");
}
