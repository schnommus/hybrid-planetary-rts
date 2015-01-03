#include <Artemis\Artemis.h>
#include <SFML\Graphics.hpp>
#include <iostream>

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

class PlayerComponent : public artemis::Component {
public:
	PlayerComponent () {}
};

class PlayerSystem : public artemis::EntityProcessingSystem {
	artemis::ComponentMapper<UVPositionComponent> positionMapper;
	artemis::ComponentMapper<PlayerComponent> playerMapper;
public:
	PlayerSystem() {
		addComponentType<UVPositionComponent>();
		addComponentType<PlayerComponent>();
	}

	virtual void initialize() {
		positionMapper.init(*world);
		playerMapper.init(*world);
	}

	virtual void processEntity(artemis::Entity &e) {
		float &u = positionMapper.get(e)->u;
		float &v = positionMapper.get(e)->v;

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			v += world->getDelta();
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			v -= world->getDelta();
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			u -= world->getDelta();
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			u += world->getDelta();

		if( u > 1.0 ) { 
			u -= 1.0;
			v = 1.0-v;
		}
		if( u < 0.0 ) { 
			u += 1.0;
			v = 1.0-v;
		}
		if( v > 1.0 ) v -= 1.0;
		if( v < 0.0 ) v += 1.0;
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

		if( !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			window.draw( s );
	}


};

class UVSphericalRenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper<UVPositionComponent> positionMapper;
	artemis::ComponentMapper<SpriteComponent> spriteMapper;
	sf::RenderWindow &window;
	float theta, gamma;
	Matrix4x3 worldtransform;
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
	};

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
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			sz = 400;
		}

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
		
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			float dist = sqrt( rotated.x * rotated.x + rotated.y * rotated.y );
			float fact = 0.35;
			if( dist > sz*0.9) fact = 0.2;
			if( dist > sz*0.95) fact = 0.1;
			s.setScale(fact*defaultScale.x, fact*defaultScale.y);
		}
		if( sunrotated.z < 0.0f) {
			float factor = 255-(-1.5*sunrotated.z/(sz/200))/2;
			if(factor < 0) factor = 0;
			s.setColor(sf::Color(factor, factor, factor, 255));
		}

		positionMapper.get(e)->screen_x = s.getPosition().x;
		positionMapper.get(e)->screen_y = s.getPosition().y;
		positionMapper.get(e)->colour = s.getColor();
		
		if( e.getComponent<TerrainNodeComponent>() != nullptr ) {
			if( !sf::Keyboard::isKeyPressed( sf::Keyboard::Space ))
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

#ifdef MINIMAP
		window.draw( s );
#endif MINIMAP

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
	PlayerSystem * playerSys = (PlayerSystem*)sm->setSystem(new PlayerSystem());

    sm->initializeAll();

    PlaceRandom(em, 70, "Desert1.png", AvoidPolarRestrict, true);
	PlaceRandom(em, 50, "Desert2.png", AvoidPolarRestrict, true);
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

	artemis::Entity & player = em->create();
	player.addComponent(new PlayerComponent() );
	player.addComponent(new UVPositionComponent(0.2, 0.2));
	player.addComponent(new SpriteComponent("point3.png"));
	player.refresh();

	sf::Clock clock;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		//logic
		world.loopStart();
		world.setDelta( clock.restart().asSeconds() );
		playerSys->process();

		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			window.clear(sf::Color::Black);
			sf::CircleShape c(402);
			c.setOrigin(c.getLocalBounds().width/2, c.getLocalBounds().height/2);
			c.setFillColor(sf::Color(66, 55, 42));
			c.setPosition( window.getSize().x/2+1, window.getSize().y/2 );
			window.draw(c);
		} else {
			window.clear(sf::Color(66, 55, 42));
		}

		// draw everything here...
		flatRenderSys->process();
		sphereRenderSys->process();

		window.display();
	}

    return 0;
}