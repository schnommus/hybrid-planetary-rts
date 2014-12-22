#include <Artemis\Artemis.h>
#include <SFML\Graphics.hpp>
#include <iostream>

#include <..\gamemath\Matrix4x3.h>
#include <..\gamemath\EulerAngles.h>
#include <..\gamemath\vector3.h>


class UVPositionComponent : public artemis::Component {
public:
	float u, v;

    UVPositionComponent(float uv, float vv) {
        this->u = uv;
        this->v = vv;
    };
};

class SpriteComponent : public artemis::Component {
public:
	sf::Sprite sprite;

	SpriteComponent( std::string directory, float scale = 1.0f ) {
		texture.loadFromFile("..\\media\\" + directory);
		sprite.setTexture(texture);
		sprite.setScale(scale, scale);
		sprite.setOrigin( sprite.getLocalBounds().width/2, sprite.getLocalBounds().height/2 );
		sprite.setRotation(rand()%360);
	}

private:
	sf::Texture texture;
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
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			gamma -= world->getDelta()/10000;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			gamma += world->getDelta()/10000;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			theta -= world->getDelta()/10000;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			theta += world->getDelta()/10000;

		// Must do this because we just overwrote the default behavior
		artemis::EntityProcessingSystem::processEntities(bag);

		gamma /= 1.1;
		theta /= 1.1;
	}

	virtual void processEntity(artemis::Entity &e) {
		float sz = 1400.0;
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			sz = 400;
		}

		float u = positionMapper.get(e)->u, v = positionMapper.get(e)->v;
		float x = cos(3.14159*u) * sin(3.14159*-2*v) * sz;
		float y = sin(3.14159*u) * sin(3.14159*-2*v) * sz;
		float z = cos(3.14159*-2*v) * sz;

		Matrix4x3 m1;
		m1.setupRotate( 1, theta );

		Matrix4x3 m2;
		m2.setupRotate( 2, gamma );

		worldtransform = worldtransform * (m1*m2);

		Vector3 rotated = Vector3(x, y, z) * worldtransform;

		Matrix4x3 sun;
		sun.setupLocalToParent( Vector3(0, 0, 0), EulerAngles(0.3, 1.8, 0) );
		Vector3 sunrotated = Vector3(x, y, z) * sun;


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
			float factor = 255-(30-1.5*sunrotated.z/(sz/200));
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

		//minimap
		if( v > 0.5 )
			s.setPosition( u*200, v*200-100 ); 
		else
			s.setPosition( u*200+200, -v*200+100 );

		s.setScale(0.1*defaultScale.x, 0.1*defaultScale.y);

		window.draw( s );

		s.setScale( defaultScale );
	};
};

int main(int argc, char **argv) {
	
    artemis::World world;
    artemis::SystemManager * sm = world.getSystemManager();
    artemis::EntityManager * em = world.getEntityManager();

	sf::RenderWindow window(sf::VideoMode(1280, 720), "My window");
	UVSphericalRenderSystem * sphereRenderSys = (UVSphericalRenderSystem*)sm->setSystem(new UVSphericalRenderSystem(window));
	PlayerSystem * playerSys = (PlayerSystem*)sm->setSystem(new PlayerSystem());

    sm->initializeAll();

	std::vector< sf::Vector2f > previous;
	previous.push_back(sf::Vector2f(0,0));
	int c = 0;
    for(int i = 0; i != 2000; ++i) {
		float u = 0, v = 0;
		for( int j = 0; j != 100; ++j ) {
			u = float(rand()%1000)/1000.0f;
			v = acos( 2*(float(rand()%1000)/1000.0f) - 1)/3.14159;

			bool close = false;
			for(int k = 0; k != previous.size(); ++k) {
				if( (previous[k].x - u)*(previous[k].x - u) + (previous[k].y - v)*(previous[k].y - v) < 0.0002 )
					close = true;
			}

			if (!close) {
				previous.push_back( sf::Vector2f(u, v));
				break;
			} else {
				u = 0;
				v = 0;
			}
		}

		if( u == 0 && v == 0) c += 1;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("moon1.png", 4.0f));
		player.refresh();
	}

	std::cout << "Unnaccounted: " << c << std::endl;

	for(int i = 0; i != 100; ++i) {
		float u = float(rand()%1000)/1000.0f, v = float(rand()%1000)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("crater.png", 4.0f));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("ice.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = 0.5+float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("ice.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = 0.5-float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("ice.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = 1-float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("ice.png"));
		player.refresh();
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
		sphereRenderSys->process();

		window.display();
	}

    return 0;
}