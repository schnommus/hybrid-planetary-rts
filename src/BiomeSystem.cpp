#include "BiomeSystem.h"
#include "Components.h"
#include "EntityFactory.h"
#include "ResourceManager.h"


BiomeSystem::BiomeSystem(Game &gamev) : game(gamev) {
	addComponentType<TransformOnBiome>();
	atmosphere = temperature = 0.0f;
	currentBiome = "temperate";
	newBiome = false;
	biomeText.setFont( ResourceManager::Inst().GetFont("RiskofRainFont.ttf") );
	biomeText.setCharacterSize(8);
	biomeCursor.setTexture(ResourceManager::Inst().GetTexture("cursor2.png"));
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
	// Debug biome changes
	float fact = world->getDelta();
	if( sf::Keyboard::isKeyPressed(sf::Keyboard::W)) atmosphere += fact;
	if( sf::Keyboard::isKeyPressed(sf::Keyboard::S)) atmosphere -= fact;
	if( sf::Keyboard::isKeyPressed(sf::Keyboard::A)) temperature -= fact;
	if( sf::Keyboard::isKeyPressed(sf::Keyboard::D)) temperature += fact;

	// Clamping parameters
	if( temperature > 1.0f ) temperature = 1.0f;
	if( temperature < -1.0f ) temperature = -1.0f;
	if( atmosphere > 1.0f ) atmosphere = 1.0f;
	if( atmosphere < -1.0f ) atmosphere = -1.0f;

	// Calculate current biome
	float theta = 180*atan2(atmosphere, temperature)/M_PI;
	float dist = sqrt( pow(atmosphere, 2) + pow( temperature, 2 ) );
	float inner = 0.57f;
	float outer = 1.05f;
	if( dist < inner) SetBiome("temperate");
	else if( dist < outer && -45 < theta && theta < 45 ) SetBiome("tropical");
	else if( dist < outer && 45 < theta && theta < 135 ) SetBiome("water");
	else if( dist < outer && -135 < theta && theta < -45 ) SetBiome("arid");
	else if( dist < outer && (theta < -135 || theta > 135) ) SetBiome("icy");
	else if( 0 < theta && theta < 90) SetBiome("sulfuric");
	else if( 90 < theta && theta < 180) SetBiome("gas");
	else if( -90 < theta && theta < 0 ) SetBiome("mineral");
	else if( -180 < theta && theta < -90 ) SetBiome("barren");
}

void BiomeSystem::DrawBiomeInfo() {
	std::string t, &b(currentBiome), s="\t\t\t| ";
	if( b=="temperate") t = "M"+s+"Temperate";
	if( b=="icy") t = "P"+s+"Glaciated";
	if( b=="tropical") t = "K"+s+"Supertropic";
	if( b=="water") t = "O"+s+"Oceanic";
	if( b=="arid") t = "H"+s+"Arid";
	if( b=="gas") t = "J"+s+"Gaseous";
	if( b=="sulfuric") t = "Y"+s+"Sulfuric";
	if( b=="mineral") t = "G"+s+"Crystalline";
	if( b=="barren") t = "L"+s+"Barren";

	biomeText.setString(t);
	biomeText.setPosition(68, 215);
	game.Renderer()->draw(biomeText);

	sf::Sprite c( ResourceManager::Inst().GetTexture("class.png"));
	c.setPosition(76, 215);
	game.Renderer()->draw(c);

	biomeCursor.setPosition(28+(27*temperature), game.Renderer()->getSize().y-35-(27*atmosphere));
	game.Renderer()->draw(biomeCursor);
}
