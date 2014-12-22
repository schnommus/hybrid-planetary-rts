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

	SpriteComponent( std::string directory ) {
		texture.loadFromFile("..\\media\\" + directory);
		sprite.setTexture(texture);
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
	float theta, phi, gamma;
	Matrix4x3 worldtransform;
public:
	UVSphericalRenderSystem( sf::RenderWindow &rwindow ) : window(rwindow) {
		addComponentType<UVPositionComponent>();
		addComponentType<SpriteComponent>();
	}

	virtual void initialize() {
		positionMapper.init(*world);
		spriteMapper.init(*world);
		theta = phi = gamma = 0.0f;
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
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::O))
			phi += world->getDelta();
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::L))
			phi -= world->getDelta();

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
		s.setPosition( rotated.x+window.getSize().x/2, rotated.y+window.getSize().y/2 );
		s.setColor(sf::Color(255, 255, 255, 255));
		s.setScale(1, 1);
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) s.setScale(0.35, 0.35);
		if( sunrotated.z < 0.0f) {
			float factor = 255-(30-sunrotated.z/(sz/200));
			s.setColor(sf::Color(factor, factor, factor, 255));
		}
		if( rotated.z > 0.0f ) {
			window.draw( s );
		}

		s.setColor(sf::Color(255, 255, 255, 64));
		if( s.getPosition().x > 0 && s.getPosition().y > 0 && s.getPosition().x < window.getSize().x && s.getPosition().y < window.getSize().y && rotated.z > 0.0f) {
			s.setColor(sf::Color(255, 255, 255, 255));
		}

		//minimap
		if( v > 0.5 )
			s.setPosition( u*200, v*200-100 ); 
		else
			s.setPosition( u*200+200, -v*200+100 );

		s.setScale(0.1, 0.1);

		window.draw( s );
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

    for(int i = 0; i != 1800; ++i) {
		float u = float(rand()%1000)/1000.0f, v = float(rand()%1000)/1000.0f;
		
		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("dirt.png"));
		player.refresh();
	}

	for(int i = 0; i != 100; ++i) {
		float u = float(rand()%1000)/1000.0f, v = float(rand()%1000)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("hole.png"));
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
			sf::CircleShape c(410);
			c.setOrigin(c.getLocalBounds().width/2, c.getLocalBounds().height/2);
			c.setFillColor(sf::Color(66, 55, 42));
			c.setPosition( window.getSize().x/2+8, window.getSize().y/2 );
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