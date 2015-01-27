#include "BiomeSystem.h"
#include "Components.h"
#include "EntityFactory.h"


BiomeSystem::BiomeSystem(Game &gamev) : game(gamev) {
	addComponentType<TransformOnBiome>();
	currentBiome = "temperate";
	newBiome = false;
}

void BiomeSystem::processEntity( artemis::Entity & e ) {
	if(newBiome) {
		TransformOnBiome &t = FetchComponent<TransformOnBiome>(e);
		for( int i = 0; i != t.types.size(); ++i ) {
			if(t.types[i]==currentBiome) {
				UVPositionComponent &u = FetchComponent<UVPositionComponent>(e);
				artemis::Entity &ent = *game.EntityFactory()->Create( t.newentities[i] );
				FetchComponent<UVPositionComponent>(ent) = u;
				e.remove();
				break;
			}
		}
	}
}

void BiomeSystem::processEntities( artemis::ImmutableBag<artemis::Entity*> &bag ) {
	DoBiomeChecks();

	artemis::EntityProcessingSystem::processEntities(bag);

	if( newBiome ) {
		game.RecalculateTerrain();
		newBiome = false;
	}
}

void BiomeSystem::SetBiome( std::string biome ) {
	if ( biome != currentBiome ) {
		newBiome = true;
		currentBiome = biome;
	}
}

void BiomeSystem::DoBiomeChecks() {
	if( sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
		SetBiome("temperate");
	}

	if( sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
		SetBiome("icy");
	}

	if( sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
		SetBiome("tropical");
	}
}
