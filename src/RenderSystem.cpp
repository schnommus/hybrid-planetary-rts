#include "RenderSystem.h"

#include "CameraSystem.h"

#include <../gamemath/vector3.h>

inline Vector3 DoUVTransform (float u, float v, float sz, Matrix4x3 & world)
{
	float x = cos(3.14159*u) * sin(3.14159*-2*v) * sz;
	float y = sin(3.14159*u) * sin(3.14159*-2*v) * sz;
	float z = cos(3.14159*-2*v) * sz;

	return Vector3(x, y, z) * world;
}


void BackgroundTerrainRenderSystem::addNodeID (int id) {nodeIds.push_back(id);}
void BackgroundTerrainRenderSystem::clearNodeIDs () {nodeIds.clear();}

BackgroundTerrainRenderSystem::BackgroundTerrainRenderSystem (sf::RenderTarget & rwindow, CameraSystem & cameraSystemv)
	: window (rwindow), cameraSystem (cameraSystemv) {
	addComponentType<FlatPositionComponent>();
	addComponentType<SpriteComponent>();
	addComponentType<BackgroundTerrainComponent>();
}

void BackgroundTerrainRenderSystem::initialize () {
	positionMapper.init(*world);
	spriteMapper.init(*world);
	initialized = false;
}

void BackgroundTerrainRenderSystem::processEntities (artemis::ImmutableBag <artemis::Entity*> & bag) {
	// Must do this because we just overwrote the default behavior
	artemis::EntityProcessingSystem::processEntities(bag);

	initialized = true;
}

void BackgroundTerrainRenderSystem::reSprite (artemis::Entity & e) {
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
	s.setColor( ( (UVPositionComponent*)world->getEntityManager()->getEntity(nodeIds[current]).getComponent<UVPositionComponent>())->colour );
}

void BackgroundTerrainRenderSystem::processEntity (artemis::Entity & e) {
	sf::Sprite &s = spriteMapper.get(e)->sprite;
	float &x = positionMapper.get(e)->x;
	float &y = positionMapper.get(e)->y;

	x += cameraSystem.dgamma*500;
	y += cameraSystem.dtheta*500;

	// If terrain goes off screen; put it back on other side of screen with relevant sprite
	if( x > window.getSize().x+16 || y > window.getSize().y+16 || x < -16 || y < -16 ) {
		if( x > window.getSize().x+16 ) x -= window.getSize().x+32;
		if( y > window.getSize().y+16 ) y -= window.getSize().y+32;
		if( x < -16 ) x += window.getSize().x+32;
		if( y < -16 ) y += window.getSize().y+32;

		reSprite(e);
	}

	if( !initialized ) {
		reSprite(e);
	}

	s.setPosition( x, y );

	window.draw( s );
}

UVSphericalRenderSystem::UVSphericalRenderSystem (sf::RenderTarget & rwindow, CameraSystem & cameraSystemv, BackgroundTerrainRenderSystem & flatSystemv)
	: window (rwindow), cameraSystem (cameraSystemv), flatSystem (flatSystemv) {
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

	Vector3 rotated = DoUVTransform( positionMapper.get(e)->u, positionMapper.get(e)->v, sz, cameraSystem.worldtransform );
	Vector3 sunrotated = DoUVTransform( positionMapper.get(e)->u, positionMapper.get(e)->v, sz, cameraSystem.sun);

	spriteMapper.get(e)->UpdateAnimation();

	sf::Sprite &s = spriteMapper.get(e)->sprite;

	// We fiddle with scale a bit here; so preserve it to re-apply at end
	sf::Vector2f defaultScale = s.getScale();

	// Set the sprite's position, default colour before sun
	s.setPosition( rotated.x+window.getSize().x/2, rotated.y+window.getSize().y/2 );
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
		int threshold = 100;
		// If on the screen
		if( s.getPosition().x > -threshold && s.getPosition().y > -threshold && s.getPosition().x < window.getSize().x+threshold && s.getPosition().y < window.getSize().y +threshold) {

			// Terrain nodes are invisible on this renderer
			if( e.getComponent<TerrainNodeComponent>() != nullptr ) {
				// (Cleared at the beginning of every frame in processentities)
				flatSystem.addNodeID(e.getId());
			} else {
				window.draw( s );
			}

			positionMapper.get(e)->on_screen = true;
		}
	} else {
		positionMapper.get(e)->on_screen = false;
	}

	s.setScale( defaultScale );
}