#include "MinimapSystem.h"
#include "CameraSystem.h"
#include "RenderSystem.h"
#include "ResourceManager.h"

#include <../gamemath/vector3.h>

StarSystem::StarSystem (Game &gamev)
	: game(gamev) {
	addComponentType<StarTag>();
	addComponentType<FlatPositionComponent>();
}

void StarSystem::initialize () {
	positionMapper.init(*world);

	// Create all the stars
	for(int i = 0; i != 50; ++i ) {
		artemis::Entity &e = world->createEntity();
		e.addComponent(new FlatPositionComponent(game.Renderer()->getSize().x-(rand()%220)/4-36/4, game.Renderer()->getSize().y-(rand()%220)/4-36/4));
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

	game.Renderer()->draw( &starPoints[0], starPoints.size(), sf::Points );
}

void StarSystem::processEntity (artemis::Entity & e) {
	float &x = positionMapper.get(e)->x;
	float &y = positionMapper.get(e)->y;

	x -= game.Camera()->dgamma*25;
	y -= game.Camera()->dtheta*25;

	auto &r = *game.Renderer();
	if( x > r.getSize().x-15/4) x = r.getSize().x-244/4;
	if( y > r.getSize().y-16/4) y = r.getSize().y-241/4;
	if( y < r.getSize().y-241/4 ) y = r.getSize().y-16/4;
	if( x < r.getSize().x-244/4 ) x = r.getSize().x-15/4;

	if( pow(y-(r.getSize().y-32), 2) + pow(x-(r.getSize().x-32), 2) > 26*26 ) {
		starPositions.push_back( sf::Vector2f(x, y) );
	}
}


MinimapSphericalRenderSystem::MinimapSphericalRenderSystem (Game &gamev)
	: game(gamev), planet_overlay( ResourceManager::Inst().GetTexture("uiOverlay.png") ) {
	addComponentType<UVPositionComponent>();
	addComponentType<SpriteComponent>();
	addComponentType<MinimapTag>();

	sz = 25;

	planet_overlay.setScale(1, 1);
	planet_overlay.setPosition(game.Renderer()->getSize().x-64, game.Renderer()->getSize().y-63);
}

void MinimapSphericalRenderSystem::initialize () {
	positionMapper.init(*world);
	spriteMapper.init(*world);
	minimapMapper.init(*world);

	starSys = (StarSystem*)world->getSystemManager()->setSystem(new StarSystem(game));
}

void MinimapSphericalRenderSystem::drawStars() {
	//Overlay is black background for stars to draw on
	game.Renderer()->draw(planet_overlay);
	starSys->process();
}

void MinimapSphericalRenderSystem::processEntity (artemis::Entity & e) {
	Vector3 rotated = DoUVTransform( positionMapper.get(e)->u, positionMapper.get(e)->v, sz, game.Camera()->worldtransform );
	Vector3 sunrotated = DoUVTransform( positionMapper.get(e)->u, positionMapper.get(e)->v, sz,  game.Camera()->sun);

	spriteMapper.get(e)->UpdateAnimation();

	sf::Sprite &s = spriteMapper.get(e)->sprite;

	// We fiddle with scale a bit here; so preserve it to re-apply at end
	sf::Vector2f defaultScale = s.getScale();

	// Set the sprite's position, default colour before sun
	s.setPosition( rotated.x+game.Renderer()->getSize().x-125/4, rotated.y+game.Renderer()->getSize().y-125/4 );
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
		if( s.getPosition().x > -threshold && s.getPosition().y > -threshold && s.getPosition().x < game.Renderer()->getSize().x+threshold && s.getPosition().y < game.Renderer()->getSize().y +threshold) {
			game.Renderer()->draw( s );
		}
	} 

	s.setScale( defaultScale );
}
