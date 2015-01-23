#include "MinimapSystem.h"

#include "RenderSystem.h"

#include <../gamemath/vector3.h>

StarSystem::StarSystem (sf::RenderTarget & rwindow, CameraSystem & cameraSystemv)
	: window (rwindow), cameraSystem (cameraSystemv) {
	addComponentType<StarTag>();
	addComponentType<FlatPositionComponent>();
}

void StarSystem::initialize () {
	positionMapper.init(*world);

	// Create all the stars
	for(int i = 0; i != 50; ++i ) {
		artemis::Entity &e = world->createEntity();
		e.addComponent(new FlatPositionComponent(window.getSize().x-(rand()%220)/4-36/4, window.getSize().y-(rand()%220)/4-36/4));
		e.addComponent(new StarTag() );
		e.refresh();
	}
}

void StarSystem::processEntities (artemis::ImmutableBag <artemis::Entity*> & bag) {
	starPositions.clear();

	// Must do this because we just overwrote the default behaviour
	artemis::EntityProcessingSystem::processEntities(bag);
	
	
	// Draw them all using a vertex buffer
	std::vector< sf::Vertex > starPoints;
	for( int i = 0; i != starPositions.size(); ++i ) {
		starPoints.push_back(sf::Vertex( starPositions[i], sf::Color::White ) );
	}

	window.draw( &starPoints[0], starPoints.size(), sf::Points );
}

void StarSystem::processEntity (artemis::Entity & e) {
	float &x = positionMapper.get(e)->x;
	float &y = positionMapper.get(e)->y;

	x -= cameraSystem.dgamma*25;
	y -= cameraSystem.dtheta*25;

	if( x > window.getSize().x-15/4) x = window.getSize().x-244/4;
	if( y > window.getSize().y-16/4) y = window.getSize().y-241/4;
	if( y < window.getSize().y-241/4 ) y = window.getSize().y-16/4;
	if( x < window.getSize().x-244/4 ) x = window.getSize().x-15/4;

	if( pow(y-(window.getSize().y-32), 2) + pow(x-(window.getSize().x-32), 2) > 26*26 ) {
		starPositions.push_back( sf::Vector2f(x, y) );
	}
}


MinimapSphericalRenderSystem::MinimapSphericalRenderSystem (sf::RenderTarget & rwindow, CameraSystem & cameraSystemv)
	: window (rwindow), cameraSystem (cameraSystemv) {
	addComponentType<UVPositionComponent>();
	addComponentType<SpriteComponent>();
	addComponentType<MinimapTag>();

	sz = 25;
}

void MinimapSphericalRenderSystem::initialize () {
	positionMapper.init(*world);
	spriteMapper.init(*world);
	minimapMapper.init(*world);

	starSys = (StarSystem*)world->getSystemManager()->setSystem(new StarSystem(window, cameraSystem));
}

void MinimapSphericalRenderSystem::drawStars() {
	starSys->process();
}

void MinimapSphericalRenderSystem::processEntity (artemis::Entity & e) {
	Vector3 rotated = DoUVTransform( positionMapper.get(e)->u, positionMapper.get(e)->v, sz, cameraSystem.worldtransform );
	Vector3 sunrotated = DoUVTransform( positionMapper.get(e)->u, positionMapper.get(e)->v, sz, cameraSystem.sun);

	spriteMapper.get(e)->UpdateAnimation();

	sf::Sprite &s = spriteMapper.get(e)->sprite;

	// We fiddle with scale a bit here; so preserve it to re-apply at end
	sf::Vector2f defaultScale = s.getScale();

	// Set the sprite's position, default colour before sun
	s.setPosition( rotated.x+window.getSize().x-125/4, rotated.y+window.getSize().y-125/4 );
	s.setColor(sf::Color(255, 255, 255, 255));

	//Reduction in size at edges
	float dist = sqrt( rotated.x * rotated.x + rotated.y * rotated.y );
	float fact = 0.35;
	if( dist > sz*0.9) fact = 0.25;
	if( dist > sz*0.95) fact = 0.15;
	s.setScale(fact*defaultScale.x, fact*defaultScale.y);

	// Terrain pieces need to appear bit bigger than normal things
	if( e.getComponent<TerrainNodeTag>() != nullptr ) {
		s.scale(1.5f, 1.5f);
	} else {
		s.scale(0.3f, 0.3f);
	}

	// Darken if on other side of sun
	if( sunrotated.z < 0.0f) {
		float factor = 255-(-1.5*sunrotated.z/(sz/200))/2;
		if(factor < 0) factor = 0;
		s.setColor(sf::Color(factor, factor, factor, 255));
	}

	if( rotated.z > 0.0f ) {
		int threshold = 100;
		if( s.getPosition().x > -threshold && s.getPosition().y > -threshold && s.getPosition().x < window.getSize().x+threshold && s.getPosition().y < window.getSize().y +threshold) {
			window.draw( s );
		}
	} 

	s.setScale( defaultScale );
}
