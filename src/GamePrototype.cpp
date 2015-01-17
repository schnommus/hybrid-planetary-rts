#include <Artemis\Artemis.h>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <..\gamemath\Matrix4x3.h>
#include <..\gamemath\EulerAngles.h>
#include <..\gamemath\vector3.h>

class UVPositionComponent : public artemis::Component {
public:
	float u, v;
	float screen_x, screen_y;
	bool on_screen;
	sf::Color colour;

    UVPositionComponent(float uv, float vv) {
        this->u = uv;
        this->v = vv;
		screen_x = screen_y = 0.0f;
		colour = sf::Color(255, 255, 255, 255);
		on_screen = false;
    };
};

class FlatPositionComponent : public artemis::Component {
public:
	float x, y;

	FlatPositionComponent(float xv, float yv) {
		this->x = xv;
		this->y = yv;
	};
};

class SpriteComponent : public artemis::Component {
public:
	sf::Sprite sprite;
	sf::Clock frameClock;
	int nFrames;

	SpriteComponent( std::string directory, float scale = 1.0f, int frames = 1 ) : nFrames(frames) {
		texture.loadFromFile("..\\media\\" + directory);
		sprite.setTexture(texture);
		sprite.setTextureRect( sf::IntRect((rand()%nFrames)*sprite.getLocalBounds().width/frames, 0, sprite.getLocalBounds().width/frames, sprite.getLocalBounds().height ) );
		sprite.setScale(scale, scale);
		sprite.setOrigin( sprite.getLocalBounds().width/2, sprite.getLocalBounds().height/2 );
	}


	void UpdateAnimation() {
		if(  nFrames != 1 && frameClock.getElapsedTime().asSeconds() > 0.1f ) {
			frameClock.restart();

			int x = sprite.getTextureRect().left;
			x += sprite.getLocalBounds().width;
			if( x >= sprite.getTexture()->getSize().x )
				x = 0;

			sprite.setTextureRect( sf::IntRect(x, 0, sprite.getLocalBounds().width, sprite.getLocalBounds().height ) );
		}
	}

private:
	sf::Texture texture;
};

class TerrainNodeComponent : public artemis::Component {
public:
	std::string type;

	TerrainNodeComponent( std::string typev )
		: type(typev) {}
};

class MinimapComponent : public artemis::Component {
public:
	MinimapComponent() {}
};

class StarComponent : public artemis::Component {
public:
	StarComponent() {}
};


class PlayerComponent : public artemis::Component {
public:
	PlayerComponent () {}
};

// This is just a helper for readability on systems that don't actually process any entities
namespace artemis {
	class ProcessingSystem : public artemis::EntityProcessingSystem {
	private:
		virtual void processEntities(artemis::ImmutableBag<artemis::Entity*> & bag) {
			doProcessing();
		}

		virtual void processEntity(artemis::Entity &e) {}
	protected:
		virtual void doProcessing() = 0;
	};
}

class CameraSystem : public artemis::ProcessingSystem {
public:
	float dtheta, dgamma;

	Matrix4x3 worldtransform, sun;
protected:
	virtual void doProcessing() {
		// Movement acc/decceleration
		dtheta *= 0.98;
		dgamma *= 0.98;

		// Update camera
		float fact = 0.07*world->getDelta()/2.0f;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			dgamma -= fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			dgamma += fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			dtheta += fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			dtheta -= fact;

		// Update matrices
		Matrix4x3 m1;
		m1.setupRotate( 1, -dtheta );

		Matrix4x3 m2;
		m2.setupRotate( 2, dgamma );

		worldtransform = worldtransform * (m1*m2);
	}

	virtual void initialize() {
		dtheta=dgamma=0.0f;
		worldtransform.identity();
		sun.setupLocalToParent( Vector3(0, 0, 0), EulerAngles(0.3, 1.8, 0) );
	}

private:
	float otheta, ogamma;
};

class DrawFPSSystem : public artemis::ProcessingSystem {
public:
	DrawFPSSystem( sf::RenderTarget &rwindow) : window(rwindow) { }

protected:
	virtual void doProcessing() {
		std::ostringstream oss;
		oss << int(1.0f/world->getDelta()) << " FPS";
		fps_text.setString(oss.str());
		window.draw(fps_text);
	}

	virtual void initialize() {
		debugfont.loadFromFile("..//media//RiskofRainFont.ttf");
		fps_text.setFont( debugfont );
		fps_text.setCharacterSize(8);
	}

private:
	sf::Text fps_text;
	sf::Font debugfont;
	sf::RenderTarget &window;
};

class StarSystem : public artemis::EntityProcessingSystem {
	artemis::ComponentMapper<FlatPositionComponent> positionMapper;
	artemis::ComponentMapper<StarComponent> starMapper;

	sf::Texture starTex;
	sf::Sprite starSprite;

	sf::RenderTarget &window;
	CameraSystem &cameraSystem;
public:
	StarSystem( sf::RenderTarget &rwindow, CameraSystem &cameraSystemv ) : window(rwindow), cameraSystem(cameraSystemv) {
		addComponentType<StarComponent>();
		addComponentType<FlatPositionComponent>();
		starTex.loadFromFile("..\\media\\star.png");
		starSprite.setTexture(starTex);
	}

	virtual void initialize() {
		positionMapper.init(*world);
		starMapper.init(*world);
	}

	virtual void processEntity(artemis::Entity &e) {
		float &x = positionMapper.get(e)->x;
		float &y = positionMapper.get(e)->y;

		x -= cameraSystem.dgamma*25;
		y -= cameraSystem.dtheta*25;

		if( x > window.getSize().x-15/4) x = window.getSize().x-244/4;
		if( y > window.getSize().y-16/4) y = window.getSize().y-241/4;
		if( y < window.getSize().y-241/4 ) y = window.getSize().y-16/4;
		if( x < window.getSize().x-244/4 ) x = window.getSize().x-15/4;

		if( pow(y-(window.getSize().y-32), 2) + pow(x-(window.getSize().x-32), 2) > 26*26 ) {
			starSprite.setPosition(x,y);
			window.draw(starSprite);
		}
	}
};

class FlatRenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper<FlatPositionComponent> positionMapper;

	artemis::ComponentMapper<SpriteComponent> spriteMapper;
	sf::RenderTarget &window;
	CameraSystem &cameraSystem;

	std::vector<int> nodeIds;
	bool initialized;
public:
	void addNodeID(int id) {nodeIds.push_back(id);}
	void clearNodeIDs() {nodeIds.clear();}

	FlatRenderSystem( sf::RenderTarget &rwindow, CameraSystem &cameraSystemv ) : window(rwindow), cameraSystem(cameraSystemv) {
		addComponentType<FlatPositionComponent>();
		addComponentType<SpriteComponent>();
	}

	virtual void initialize() {
		positionMapper.init(*world);
		spriteMapper.init(*world);
		initialized = false;
	};

	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*> & bag) {
		// Must do this because we just overwrote the default behavior
		artemis::EntityProcessingSystem::processEntities(bag);

		initialized = true;
	}

	void reSprite(artemis::Entity &e) {
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

	virtual void processEntity(artemis::Entity &e) {
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


};

inline Vector3 DoUVTransform( float u, float v, float sz, Matrix4x3 &world) {
	float x = cos(3.14159*u) * sin(3.14159*-2*v) * sz;
	float y = sin(3.14159*u) * sin(3.14159*-2*v) * sz;
	float z = cos(3.14159*-2*v) * sz;

	return Vector3(x, y, z) * world;
}

class MinimapSphericalRenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper<UVPositionComponent> positionMapper;
	artemis::ComponentMapper<SpriteComponent> spriteMapper;
	artemis::ComponentMapper<MinimapComponent> minimapMapper;

	sf::RenderTarget &window;
	CameraSystem &cameraSystem;

	float sz;
public:
	MinimapSphericalRenderSystem( sf::RenderTarget &rwindow, CameraSystem &cameraSystemv ) : window(rwindow), cameraSystem(cameraSystemv) {
		addComponentType<UVPositionComponent>();
		addComponentType<SpriteComponent>();
		addComponentType<MinimapComponent>();

		sz = 25;
	}

	virtual void initialize() {
		positionMapper.init(*world);
		spriteMapper.init(*world);
		minimapMapper.init(*world);

		// Create all the stars
		for(int i = 0; i != 50; ++i ) {
			artemis::Entity &e = world->createEntity();
			e.addComponent(new FlatPositionComponent(window.getSize().x-(rand()%220)/4-36/4, window.getSize().y-(rand()%220)/4-36/4));
			e.addComponent(new StarComponent() );
			e.refresh();
		}
	};

	virtual void processEntity(artemis::Entity &e) {
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
		if( e.getComponent<TerrainNodeComponent>() != nullptr ) {
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
	};
};

class UVSphericalRenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper<UVPositionComponent> positionMapper;
	artemis::ComponentMapper<SpriteComponent> spriteMapper;
	sf::RenderTarget &window;
	CameraSystem &cameraSystem;
	FlatRenderSystem &flatSystem;
public:
	UVSphericalRenderSystem( sf::RenderTarget &rwindow, CameraSystem &cameraSystemv, FlatRenderSystem &flatSystemv )
		: window(rwindow), cameraSystem(cameraSystemv), flatSystem(flatSystemv) {
		addComponentType<UVPositionComponent>();
		addComponentType<SpriteComponent>();
	}

	virtual void initialize() {
		positionMapper.init(*world);
		spriteMapper.init(*world);
	};

	virtual void processEntities( artemis::ImmutableBag<artemis::Entity*> &bag ) {
		flatSystem.clearNodeIDs();
		artemis::EntityProcessingSystem::processEntities(bag);
	}

	virtual void processEntity(artemis::Entity &e) {
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
	};
};

bool NoRestrict(float u, float v) {
	return true;
}

bool PolarRestrict(float u, float v) {
	return v < 0.1 || (v>0.4 && v<0.6) || v > 0.9;
}

bool AvoidPolarRestrict(float u, float v) {
	return (v > 0.1 && v < 0.4) || (v > 0.6 && v < 0.9);
}

// A restrict function will return true if the goal position is valid
void PlaceRandom(artemis::EntityManager* em, int n, std::string type, bool (*restrict)(float , float) = NoRestrict, bool isNode = false, int frames = 1 ) {
	for(int i = 0; i != n; ++i) {
		float u = float(rand()%1000)/1000.0f;
		float v = float(rand()%1000)/1000.0f;

		if( (*restrict)(u, v) == true ) {
			artemis::Entity & ent = em->create();
			ent.addComponent(new UVPositionComponent(u, v));
			ent.addComponent(new SpriteComponent(type, 1.0f, frames));
			if( isNode ) {
				ent.addComponent( new TerrainNodeComponent( type ) );
			}
			ent.addComponent(new MinimapComponent());
			ent.refresh();
		} else {
			--i; // Failed to place
		}
	}
}

int main(int argc, char **argv) {
	// Artemis containers
    artemis::World world;
    artemis::SystemManager * sm = world.getSystemManager();
    artemis::EntityManager * em = world.getEntityManager();

	// SFML rendering devices
	sf::RenderWindow realwindow(sf::VideoMode(1280, 720), "GamePrototype", sf::Style::Default );
	sf::RenderTexture window; window.create(427, 240);

	// Create all systems
	CameraSystem *cameraSys =
		(CameraSystem*)sm->setSystem(new CameraSystem());

	FlatRenderSystem * flatRenderSys =
		(FlatRenderSystem*)sm->setSystem(new FlatRenderSystem(window, *cameraSys));

	UVSphericalRenderSystem * sphereRenderSys =
		(UVSphericalRenderSystem*)sm->setSystem(new UVSphericalRenderSystem(window, *cameraSys, *flatRenderSys));

	StarSystem * starSys =
		(StarSystem*)sm->setSystem(new StarSystem(window, *cameraSys));

	MinimapSphericalRenderSystem *minimapRenderSys =
		(MinimapSphericalRenderSystem*)sm->setSystem(new MinimapSphericalRenderSystem(window, *cameraSys));

	DrawFPSSystem * fpsSys =
		(DrawFPSSystem*)sm->setSystem(new DrawFPSSystem(window));

    sm->initializeAll();

    PlaceRandom(em, 170, "Desert1.png", AvoidPolarRestrict, true);
	PlaceRandom(em, 150, "Desert2.png", AvoidPolarRestrict, true);
	PlaceRandom(em, 35, "Desert3.png", AvoidPolarRestrict, true);

	PlaceRandom(em, 20, "Snow1.png", PolarRestrict, true);
	PlaceRandom(em, 20, "Snow2.png", PolarRestrict, true);

	PlaceRandom(em, 40, "Rock1.png");
	PlaceRandom(em, 50, "AirDispensor.png", NoRestrict, false, 25);
	PlaceRandom(em, 5, "DroneDown.png", NoRestrict, false, 3);
	PlaceRandom(em, 5, "DroneLeft.png", NoRestrict, false, 3);
	PlaceRandom(em, 5, "DroneRight.png", NoRestrict, false, 3);
	PlaceRandom(em, 5, "DroneUp.png", NoRestrict, false, 3);
	PlaceRandom(em, 10, "Artefact1.png");
	PlaceRandom(em, 40, "Plant1.png", AvoidPolarRestrict);
	PlaceRandom(em, 40, "Plant2.png", AvoidPolarRestrict);

	// Add all the background terrain entities
	for(int i = 0; i != (int)(ceil(window.getSize().x/16.0f)+2); ++i) {
		for(int j = 0; j != (int)(ceil(window.getSize().y/16.0f)+2); ++j) {
			artemis::Entity & player = em->create();
			player.addComponent(new FlatPositionComponent(16*i-16, 16*j-16));
			player.addComponent(new SpriteComponent("Desert1.png", 1.005f));
			player.refresh();
		}
	}

	sf::Clock clock;

	sf::Texture uitex; uitex.loadFromFile("..\\media\\uiOverlay.png");
	sf::Sprite uispr(uitex);
	uispr.setScale(1, 1);
	uispr.setPosition(window.getSize().x-109, window.getSize().y-63);

	sf::Sprite pixelrenderer;
	pixelrenderer.setScale(3, 3);

	bool firstFrame = true;

	while (realwindow.isOpen()) {
		sf::Event event;
		while (realwindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				realwindow.close();
		}

		//logic
		world.loopStart();
		world.setDelta( clock.restart().asSeconds() );
		

		window.clear(sf::Color(66, 55, 42));

		cameraSys->process();

		// drawing
		if( firstFrame ) { // This is so that terrain nodes exist before initial spriting action
			firstFrame = false;
			sphereRenderSys->process();
		}
		flatRenderSys->process();
		sphereRenderSys->process();
		minimapRenderSys->process();

		window.draw(uispr);
		starSys->process();
		fpsSys->process();

		window.display();

		pixelrenderer.setTexture( window.getTexture() );
		realwindow.draw(pixelrenderer);
		realwindow.display();
	}

    return 0;
}