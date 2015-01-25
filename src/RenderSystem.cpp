#include "RenderSystem.h"

#include "CameraSystem.h"

#include <../gamemath/vector3.h>

#include <cmath>


Vector3 DoUVTransform (float u, float v, float sz, Matrix4x3 & world)
{
	using namespace std;
	float x = cos(M_PI*u) * sin(M_PI*-2.0f*v) * sz;
	float y = sin(M_PI*u) * sin(M_PI*-2.0f*v) * sz;
	float z = cos(M_PI*-2.0f*v) * sz;

	return Vector3(x, y, z) * world;
}

sf::Vector2f ReverseUVTransform (Vector3 in, float sz, Matrix4x3 & world) {
	using namespace std;
	in = in * inverse(world);
	sf::Vector2f out;
	
	float dx = in.x/sz;
	float dy = -in.y/sz;
	float dz = in.z/sz;
	float xydist = sqrt( dx*dx + dy*dy );
	out.x = (2.0f*atan( (xydist + dx)/dy ))/M_PI;
	out.y = atan( (1.0f-dz)/xydist )/M_PI;

	return out;
}


void BackgroundTerrainRenderSystem::addNodeID (int id) {nodeIds.push_back(id);}
void BackgroundTerrainRenderSystem::clearNodeIDs () {nodeIds.clear();}

BackgroundTerrainRenderSystem::BackgroundTerrainRenderSystem (Game &gamev)
	: game(gamev) {
	addComponentType<FlatPositionComponent>();
	addComponentType<SpriteComponent>();
	addComponentType<BackgroundTerrainTag>();
}

void BackgroundTerrainRenderSystem::initialize () {
	positionMapper.init(*world);
	spriteMapper.init(*world);

	// Add all the background terrain entities
	for(int i = 0; i != (int)(ceil(game.Renderer()->getSize().x/16.0f)+2); ++i) {
		for(int j = 0; j != (int)(ceil(game.Renderer()->getSize().y/16.0f)+2); ++j) {
			artemis::Entity & e = world->createEntity();
			e.addComponent(new FlatPositionComponent(16*i-16, 16*j-16));
			e.addComponent(new SpriteComponent("Desert1.png", 1.005f));
			e.addComponent(new BackgroundTerrainTag());
			e.refresh();
		}
	}
}

void BackgroundTerrainRenderSystem::processEntities (artemis::ImmutableBag <artemis::Entity*> & bag) {
	// Must do this because we just overwrote the default behavior
	artemis::EntityProcessingSystem::processEntities(bag);

	initialized = true;
}

void BackgroundTerrainRenderSystem::reSpriteAll() {
	initialized = false;
}

void BackgroundTerrainRenderSystem::reSprite (artemis::Entity & e) {
	if( nodeIds.empty() ) return;

	sf::Sprite &s = spriteMapper.get(e)->sprite;
	float &x = positionMapper.get(e)->x;
	float &y = positionMapper.get(e)->y;

	std::vector<float> distances;
	for(int i = 0; i != nodeIds.size(); ++i) {
		float screen_x = ((UVPositionComponent*)world->getEntityManager()->getEntity(nodeIds[i]).getComponent<UVPositionComponent>())->screen_x;
		float screen_y = ((UVPositionComponent*)world->getEntityManager()->getEntity(nodeIds[i]).getComponent<UVPositionComponent>())->screen_y;
		distances.push_back( sqrt( (screen_x-x)*(screen_x-x) + (screen_y-y)*(screen_y-y) ) );
	}

	float min = 1e9f;
	int current = -1;
	for(int i = 0; i != distances.size(); ++i) {
		if( min > distances[i] ) {
			current = i;
			min = distances[i];
		}
	}

	s.setTexture( * ( (SpriteComponent*)world->getEntityManager()->getEntity(nodeIds[current]).getComponent<SpriteComponent>())->sprite.getTexture() );
}

void BackgroundTerrainRenderSystem::processEntity (artemis::Entity & e) {
	sf::Sprite &s = spriteMapper.get(e)->sprite;
	float &x = positionMapper.get(e)->x;
	float &y = positionMapper.get(e)->y;

	x += game.Camera()->dgamma*500;
	y += game.Camera()->dtheta*500;

	// If terrain goes off screen; put it back on other side of screen with relevant sprite
	if( x > game.Renderer()->getSize().x+16 || y > game.Renderer()->getSize().y+16 || x < -16 || y < -16 ) {
		if( x > game.Renderer()->getSize().x+16 ) x -= game.Renderer()->getSize().x+32;
		if( y > game.Renderer()->getSize().y+16 ) y -= game.Renderer()->getSize().y+32;
		if( x < -16 ) x += game.Renderer()->getSize().x+32;
		if( y < -16 ) y += game.Renderer()->getSize().y+32;

		reSprite(e);
	}

	if( !initialized ) {
		reSprite(e);
	}

	//Use real position as uv index for lighting
	float sz = 500.0f;
	x -= game.Renderer()->getSize().x/2;
	y -= game.Renderer()->getSize().y/2;

	float z = sqrt( sz*sz-x*x-y*y );
	sf::Vector2f uv = ReverseUVTransform( Vector3(x, y, z), sz, game.Camera()->worldtransform );
	Vector3 sunrotated = DoUVTransform( uv.x, uv.y, sz, game.Camera()->sun);

	if( sunrotated.z < 0) {
		float factor = 255-(1.5*abs(sunrotated.z)/(sz/200))/2;
		if(factor > 255) factor = 255;
		if(factor < 0) factor = 0;
		s.setColor(sf::Color(factor, factor, factor, 255));
	} else {
		s.setColor(sf::Color(255, 255, 255, 255));
	}

	// Darken terrain around the edges slightly for a vigniette effect
	sf::Color c = s.getColor();
	float fact = pow( sqrt(x*x+y*y)/30.0f, 2);
	s.setColor( sf::Color(c.r-fact>0?c.r-fact:0, c.g-fact>0?c.g-fact:0, c.b-fact>0?c.b-fact:0, c.a) );

	x += game.Renderer()->getSize().x/2;
	y += game.Renderer()->getSize().y/2;


	s.setPosition( x, y );

	game.Renderer()->draw( s );
}

UVSphericalRenderSystem::UVSphericalRenderSystem (Game &gamev, BackgroundTerrainRenderSystem & flatSystemv)
	: game(gamev), flatSystem (flatSystemv) {
	addComponentType<UVPositionComponent>();
	addComponentType<SpriteComponent>();
}

void UVSphericalRenderSystem::initialize () {
	positionMapper.init(*world);
	spriteMapper.init(*world);
}

void UVSphericalRenderSystem::processEntities (artemis::ImmutableBag <artemis::Entity*> & bag) {
	flatSystem.clearNodeIDs();
	artemis::EntityProcessingSystem::processEntities(bag);
}

void UVSphericalRenderSystem::processEntity (artemis::Entity & e) {
	float sz = 500.0;

	Vector3 rotated = DoUVTransform( positionMapper.get(e)->u, positionMapper.get(e)->v, sz, game.Camera()->worldtransform );
	Vector3 sunrotated = DoUVTransform( positionMapper.get(e)->u, positionMapper.get(e)->v, sz, game.Camera()->sun);

	spriteMapper.get(e)->UpdateAnimation();

	sf::Sprite &s = spriteMapper.get(e)->sprite;

	// We fiddle with scale a bit here; so preserve it to re-apply at end
	sf::Vector2f defaultScale = s.getScale();

	// Set the sprite's position, default colour before sun
	s.setPosition( rotated.x+game.Renderer()->getSize().x/2, rotated.y+game.Renderer()->getSize().y/2 );
	s.setColor(sf::Color(255, 255, 255, 255));

	// Darken if on other side of sun
	if( sunrotated.z < 0.0f) {
		float factor = 255-(-1.5*sunrotated.z/(sz/200))/2;
		if(factor < 0) factor = 0;
		s.setColor(sf::Color(factor, factor, factor, 255));
	}

	// Update parameters on UV component for use in other areas
	positionMapper.get(e)->screen_x = s.getPosition().x;
	positionMapper.get(e)->screen_y = s.getPosition().y;
	positionMapper.get(e)->colour = s.getColor();

	if( rotated.z > 0.0f ) {
		int threshold = 80;
		// If on the screen
		if( s.getPosition().x > -threshold && s.getPosition().y > -threshold && s.getPosition().x < game.Renderer()->getSize().x+threshold && s.getPosition().y < game.Renderer()->getSize().y +threshold) {

			// Terrain nodes are invisible on this renderer
			if( e.getComponent<TerrainNodeTag>() != nullptr ) {
				// (Cleared at the beginning of every frame in processentities)
				flatSystem.addNodeID(e.getId());

				if( game.LevelEditorEnabled() ) { // Display terrain nodes
					s.setColor(sf::Color(255, 255, 128, 255));
					s.setRotation(45);
					s.scale(0.5f, 0.5f);
					game.Renderer()->draw( s );
					s.setRotation(0);
					s.scale(2.0f, 2.0f);
				}

			} else {
				game.Renderer()->draw( s );
			}

			positionMapper.get(e)->on_screen = true;

		}
	} else {
		positionMapper.get(e)->on_screen = false;
	}

	s.setScale( defaultScale );
}
