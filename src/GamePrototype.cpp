#include <Artemis\Artemis.h>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <..\gamemath\Matrix4x3.h>
#include <..\gamemath\EulerAngles.h>
#include <..\gamemath\vector3.h>

//#define MINIMAP

std::vector<int> nodeIds;

class UVPositionComponent : public artemis::Component {
public:
	float u, v;
	float screen_x, screen_y;
	sf::Color colour;

    UVPositionComponent(float uv, float vv) {
        this->u = uv;
        this->v = vv;
		screen_x = screen_y = 0.0f;
		colour = sf::Color(255, 255, 255, 255);
    };
};

class FlatPositionComponent : public artemis::Component {
public:
	float x, y;
	int mother;

	FlatPositionComponent(float xv, float yv) {
		this->x = xv;
		this->y = yv;
		mother = 0;
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

class StarSystem : public artemis::EntityProcessingSystem {
	artemis::ComponentMapper<FlatPositionComponent> positionMapper;
	artemis::ComponentMapper<StarComponent> starMapper;
	sf::Texture starTex;
	sf::Sprite starSprite;
	sf::RenderWindow &window;
	float theta, gamma;
	float fact;
public:
	StarSystem(sf::RenderWindow &rwindow ) : window(rwindow) {
		addComponentType<StarComponent>();
		addComponentType<FlatPositionComponent>();
		theta=gamma=0.0f;
		starTex.loadFromFile("..\\media\\star.png");
		starSprite.setTexture(starTex);
		starSprite.setScale(4, 4);
	}

	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*> & bag) {
		theta *= 0.98;
		gamma *= 0.98;

		float fact = 70*sin(world->getDelta());
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			gamma -= fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			gamma += fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			theta += fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			theta -= fact;

		// Must do this because we just overwrote the default behavior
		artemis::EntityProcessingSystem::processEntities(bag);
	}

	virtual void initialize() {
		positionMapper.init(*world);
		starMapper.init(*world);
	}

	virtual void processEntity(artemis::Entity &e) {
		float &x = positionMapper.get(e)->x;
		float &y = positionMapper.get(e)->y;

		x -= gamma/10.0;
		y -= theta/10.0;

		if( x > window.getSize().x-15) x = window.getSize().x-244;
		if( y > window.getSize().y-16) y = window.getSize().y-241;
		if( y < window.getSize().y-241 ) y = window.getSize().y-16;
		if( x < window.getSize().x-244 ) x = window.getSize().x-15;

		starSprite.setPosition(x,y);
		window.draw(starSprite);
	}
};

class FlatRenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper<FlatPositionComponent> positionMapper;
	artemis::ComponentMapper<SpriteComponent> spriteMapper;
	sf::RenderWindow &window;
	float theta, gamma;
public:
	FlatRenderSystem( sf::RenderWindow &rwindow ) : window(rwindow) {
		addComponentType<FlatPositionComponent>();
		addComponentType<SpriteComponent>();
	}

	virtual void initialize() {
		positionMapper.init(*world);
		spriteMapper.init(*world);
		theta=gamma=0.0f;
	};

	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*> & bag) {
		//theta=gamma=0.0f;
		theta *= 0.98;
		gamma *= 0.98;

		float fact = 70*sin(world->getDelta());
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			gamma -= fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			gamma += fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			theta += fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			theta -= fact;

		// Must do this because we just overwrote the default behavior
		artemis::EntityProcessingSystem::processEntities(bag);

#ifdef MINIMAP
		sf::RectangleShape rect(sf::Vector2f(405, 105));
		rect.setFillColor(sf::Color(0, 0, 0, 178));
		rect.setOutlineColor(sf::Color::White);
		rect.setOutlineThickness(1);
		rect.setPosition(1, 1);
		window.draw(rect);
#endif
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
		x += gamma;
		y += theta;
		sf::Vector2f defaultScale = s.getScale();

		if( x > window.getSize().x+64 || y > window.getSize().y+64 || x < -64 || y < -64 ) {
			if( x > window.getSize().x+64 ) x -= window.getSize().x+128;
			if( y > window.getSize().y+64 ) y -= window.getSize().y+128;
			if( x < -64 ) x += window.getSize().x+128;
			if( y < -64 ) y += window.getSize().y+128;

			reSprite(e);
		}

		s.setPosition( x, y );

		window.draw( s );
	}


};

struct vec2node {
	vec2node(int xv, int yv) : x(xv), y(yv) { }
	int x, y;
	std::vector<int> connected;
	bool operator<(const vec2node &rhs) const {
		return x < rhs.x && y < rhs.y;
	}
	bool operator==(const vec2node &rhs) const {
		return x == rhs.x && y == rhs.y;
	}
};


class MinimapSphericalRenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper<UVPositionComponent> positionMapper;
	artemis::ComponentMapper<SpriteComponent> spriteMapper;
	artemis::ComponentMapper<MinimapComponent> minimapMapper;
	sf::RenderWindow &window;
	float theta, gamma;
	Matrix4x3 worldtransform;
	sf::Font debugfont;
	float sz;
public:
	MinimapSphericalRenderSystem( sf::RenderWindow &rwindow ) : window(rwindow) {
		addComponentType<UVPositionComponent>();
		addComponentType<SpriteComponent>();
		addComponentType<MinimapComponent>();

		sz = 100;
	}

	virtual void initialize() {
		positionMapper.init(*world);
		spriteMapper.init(*world);
		minimapMapper.init(*world);
		theta = gamma = 0.0f;
		worldtransform.identity();

		debugfont.loadFromFile("..\\media\\RiskofRainFont.ttf");
	};

	sf::Vector2f transform2D(float u, float v) {
		float x = cos(3.14159*u) * sin(3.14159*-2*v) * sz;
		float y = sin(3.14159*u) * sin(3.14159*-2*v) * sz;
		float z = cos(3.14159*-2*v) * sz;

		Vector3 rotated = Vector3(x, y, z) * worldtransform;

		return sf::Vector2f( rotated.x+window.getSize().x/2, rotated.y+window.getSize().y/2);
	}

	bool isVisible(float u, float v) {
		float x = cos(3.14159*u) * sin(3.14159*-2*v) * sz;
		float y = sin(3.14159*u) * sin(3.14159*-2*v) * sz;
		float z = cos(3.14159*-2*v) * sz;

		Vector3 rotated = Vector3(x, y, z) * worldtransform;

		return rotated.z > 0.0f;
	}

	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*> & bag) {
		//theta=gamma=0.0f;

		theta *= 0.98;
		gamma *= 0.98;

		float fact = 0.07*world->getDelta()/2.0f;

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			gamma -= fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			gamma += fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			theta -= fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			theta += fact;

		Matrix4x3 m1;
		m1.setupRotate( 1, theta );

		Matrix4x3 m2;
		m2.setupRotate( 2, gamma );

		worldtransform = worldtransform * (m1*m2);

		// Must do this because we just overwrote the default behavior
		artemis::EntityProcessingSystem::processEntities(bag);
	}

	virtual void processEntity(artemis::Entity &e) {
		float u = positionMapper.get(e)->u, v = positionMapper.get(e)->v;
		float x = cos(3.14159*u) * sin(3.14159*-2*v) * sz;
		float y = sin(3.14159*u) * sin(3.14159*-2*v) * sz;
		float z = cos(3.14159*-2*v) * sz;

		Vector3 rotated = Vector3(x, y, z) * worldtransform;

		Matrix4x3 sun;
		sun.setupLocalToParent( Vector3(0, 0, 0), EulerAngles(0.3, 1.8, 0) );
		Vector3 sunrotated = Vector3(x, y, z) * sun;

		spriteMapper.get(e)->UpdateAnimation();
		sf::Sprite &s = spriteMapper.get(e)->sprite;
		sf::Vector2f defaultScale = s.getScale();

		s.setPosition( rotated.x+window.getSize().x-125, rotated.y+window.getSize().y-125 );
		s.setColor(sf::Color(255, 255, 255, 255));

		//Reduction in size at edges
		float dist = sqrt( rotated.x * rotated.x + rotated.y * rotated.y );
		float fact = 0.35;
		if( dist > sz*0.9) fact = 0.25;
		if( dist > sz*0.95) fact = 0.15;
		s.setScale(fact*defaultScale.x, fact*defaultScale.y);


		if( e.getComponent<TerrainNodeComponent>() != nullptr ) {
			s.scale(1.5f, 1.5f);
		} else {
			s.scale(0.3f, 0.3f);
		}

		if( sunrotated.z < 0.0f) {
			float factor = 255-(-1.5*sunrotated.z/(sz/200))/2;
			if(factor < 0) factor = 0;
			s.setColor(sf::Color(factor, factor, factor, 255));
		}

		if( rotated.z > 0.0f ) {
			if( s.getPosition().x > -100 && s.getPosition().y > -100 && s.getPosition().x < window.getSize().x+100 && s.getPosition().y < window.getSize().y +100) {
				window.draw( s );
			}
		} 

		s.setColor(sf::Color(255, 255, 255, 34));
		if( s.getPosition().x > 0 && s.getPosition().y > 0 && s.getPosition().x < window.getSize().x && s.getPosition().y < window.getSize().y && rotated.z > 0.0f) {
			s.setColor(sf::Color(255, 255, 255, 255));
		}

		s.setScale( defaultScale );
	};
};

class UVSphericalRenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper<UVPositionComponent> positionMapper;
	artemis::ComponentMapper<SpriteComponent> spriteMapper;
	sf::RenderWindow &window;
	float theta, gamma;
	Matrix4x3 worldtransform;
	sf::Font debugfont;

public:
	UVSphericalRenderSystem( sf::RenderWindow &rwindow ) : window(rwindow) {
		addComponentType<UVPositionComponent>();
		addComponentType<SpriteComponent>();
	}

	virtual void initialize() {
		positionMapper.init(*world);
		spriteMapper.init(*world);
		theta = gamma = 0.0f;
		worldtransform.identity();

		debugfont.loadFromFile("..\\media\\RiskofRainFont.ttf");
	};

	sf::Vector2f transform2D(float u, float v) {
		

		float sz = 2000.0;
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			sz = 400;
		}

		float x = cos(3.14159*u) * sin(3.14159*-2*v) * sz;
		float y = sin(3.14159*u) * sin(3.14159*-2*v) * sz;
		float z = cos(3.14159*-2*v) * sz;

		Vector3 rotated = Vector3(x, y, z) * worldtransform;

		return sf::Vector2f( rotated.x+window.getSize().x/2, rotated.y+window.getSize().y/2);
	}

	bool isVisible(float u, float v) {
		float sz = 2000.0;
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			sz = 400;
		}
		float x = cos(3.14159*u) * sin(3.14159*-2*v) * sz;
		float y = sin(3.14159*u) * sin(3.14159*-2*v) * sz;
		float z = cos(3.14159*-2*v) * sz;

		Vector3 rotated = Vector3(x, y, z) * worldtransform;

		return rotated.z > 0.0f;
	}

	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*> & bag) {
		//theta=gamma=0.0f;

		theta *= 0.98;
		gamma *= 0.98;

		float fact = 0.07*world->getDelta()/2.0f;

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			gamma -= fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			gamma += fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			theta -= fact;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			theta += fact;

		Matrix4x3 m1;
		m1.setupRotate( 1, theta );

		Matrix4x3 m2;
		m2.setupRotate( 2, gamma );

		worldtransform = worldtransform * (m1*m2);

		// Must do this because we just overwrote the default behavior
		artemis::EntityProcessingSystem::processEntities(bag);
	}

	virtual void processEntity(artemis::Entity &e) {
		float sz = 2000.0;

		float u = positionMapper.get(e)->u, v = positionMapper.get(e)->v;
		float x = cos(3.14159*u) * sin(3.14159*-2*v) * sz;
		float y = sin(3.14159*u) * sin(3.14159*-2*v) * sz;
		float z = cos(3.14159*-2*v) * sz;

		Vector3 rotated = Vector3(x, y, z) * worldtransform;

		Matrix4x3 sun;
		sun.setupLocalToParent( Vector3(0, 0, 0), EulerAngles(0.3, 1.8, 0) );
		Vector3 sunrotated = Vector3(x, y, z) * sun;

		spriteMapper.get(e)->UpdateAnimation();
		sf::Sprite &s = spriteMapper.get(e)->sprite;
		sf::Vector2f defaultScale = s.getScale();

		s.setPosition( rotated.x+window.getSize().x/2, rotated.y+window.getSize().y/2 );
		s.setColor(sf::Color(255, 255, 255, 255));
		
		if( sunrotated.z < 0.0f) {
			float factor = 255-(-1.5*sunrotated.z/(sz/200))/2;
			if(factor < 0) factor = 0;
			s.setColor(sf::Color(factor, factor, factor, 255));
		}

		positionMapper.get(e)->screen_x = s.getPosition().x;
		positionMapper.get(e)->screen_y = s.getPosition().y;
		positionMapper.get(e)->colour = s.getColor();
		
		if( e.getComponent<TerrainNodeComponent>() != nullptr ) {
				s.setColor(sf::Color(255, 255, 255, 0));
		}

		if( rotated.z > 0.0f ) {
			if( s.getPosition().x > -100 && s.getPosition().y > -100 && s.getPosition().x < window.getSize().x+100 && s.getPosition().y < window.getSize().y +100) {
				window.draw( s );
			}
		} else {
			positionMapper.get(e)->screen_x = 2000;
			positionMapper.get(e)->screen_y = 2000;
		}

		s.setColor(sf::Color(255, 255, 255, 34));
		if( s.getPosition().x > 0 && s.getPosition().y > 0 && s.getPosition().x < window.getSize().x && s.getPosition().y < window.getSize().y && rotated.z > 0.0f) {
			s.setColor(sf::Color(255, 255, 255, 255));
		}

		//minimap
		if( v > 0.5 )
			s.setPosition( u*200, v*200-100 ); 
		else
			s.setPosition( u*200+200, -v*200+100 );

		s.setScale(0.1*defaultScale.x, 0.1*defaultScale.y);


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
			ent.addComponent(new SpriteComponent(type, 4.0f, frames));
			if( isNode ) {
				ent.addComponent( new TerrainNodeComponent( type ) );
				nodeIds.push_back(ent.getId());
			}
			ent.addComponent(new MinimapComponent());
			ent.refresh();
		} else {
			--i;
		}
	}
}

int main(int argc, char **argv) {
	
    artemis::World world;
    artemis::SystemManager * sm = world.getSystemManager();
    artemis::EntityManager * em = world.getEntityManager();

	sf::RenderWindow window(sf::VideoMode(1280, 720), "My window", sf::Style::Default );
	UVSphericalRenderSystem * sphereRenderSys = (UVSphericalRenderSystem*)sm->setSystem(new UVSphericalRenderSystem(window));
	FlatRenderSystem * flatRenderSys = (FlatRenderSystem*)sm->setSystem(new FlatRenderSystem(window));
	StarSystem * starSys = (StarSystem*)sm->setSystem(new StarSystem(window));
	MinimapSphericalRenderSystem *minimapRenderSys = (MinimapSphericalRenderSystem*)sm->setSystem(new MinimapSphericalRenderSystem(window));

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

	for(int i = 0; i != (int)(ceil(window.getSize().x/64.0f)+2); ++i) {
		for(int j = 0; j != (int)(ceil(window.getSize().y/64.0f)+2); ++j) {
			artemis::Entity & player = em->create();
			player.addComponent(new FlatPositionComponent(64*i-64, 64*j-64));
			player.addComponent(new SpriteComponent("Desert1.png", 4.03f));
			player.refresh();
		}
	}

	for(int i = 0; i != 50; ++i ) {
		artemis::Entity &e = em->create();
		e.addComponent(new FlatPositionComponent(window.getSize().x-rand()%220-36, window.getSize().y-rand()%220-36));
		e.addComponent(new StarComponent() );
		e.refresh();
	}

	artemis::Entity & player = em->create();
	player.addComponent(new PlayerComponent() );
	player.addComponent(new UVPositionComponent(0.2, 0.2));
	player.addComponent(new SpriteComponent("point3.png"));
	player.refresh();

	sf::Clock clock;

	sf::Texture uitex; uitex.loadFromFile("..\\media\\uiOverlay.png");
	sf::Sprite uispr(uitex);
	uispr.setScale(4, 4);
	uispr.setPosition(0, window.getSize().y-4*63);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		//logic
		world.loopStart();
		world.setDelta( clock.restart().asSeconds() );

		window.clear(sf::Color(66, 55, 42));

		// draw everything here...
		flatRenderSys->process();
		sphereRenderSys->process();

		window.draw(uispr);
		starSys->process();
		minimapRenderSys->process();

		window.display();

	}

    return 0;
}