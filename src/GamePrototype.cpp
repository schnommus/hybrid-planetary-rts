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
			gamma += world->getDelta()/100;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			gamma -= world->getDelta()/100;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			theta += world->getDelta()/100;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			theta -= world->getDelta()/100;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::O))
			phi += world->getDelta();
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::L))
			phi -= world->getDelta();

		// Must do this because we just overwrote the default behavior
		artemis::EntityProcessingSystem::processEntities(bag);

		gamma = 0;
		theta = 0;
	}

	virtual void processEntity(artemis::Entity &e) {
		float u = positionMapper.get(e)->u, v = positionMapper.get(e)->v;
		float x = cos(3.14159*u) * sin(3.14159*-2*v) * 200;
		float y = sin(3.14159*u) * sin(3.14159*-2*v) * 200;
		float z = cos(3.14159*-2*v) * 200;

		Matrix4x3 m1;
		m1.setupRotate( 1, theta );

		Matrix4x3 m2;
		m2.setupRotate( 2, gamma );

		worldtransform = worldtransform * (m1*m2);

		Vector3 rotated = Vector3(x, y, z) * worldtransform;


		sf::Sprite &s = spriteMapper.get(e)->sprite;
		s.setPosition( rotated.x+200, rotated.y+200 );
		s.setColor(sf::Color(255, 255, 255, 255));
		if( rotated.z > 0.0f ) {
			window.draw( s );
		}

		//minimap
		if( v > 0.5 )
			s.setPosition( u*200+350, v*200+350 ); 
		else
			s.setPosition( u*200+550, -v*200+550 );

		if( u < 0 ) {
			std::cout << u << std::endl;
		}

		window.draw( s );
	};
};

int main(int argc, char **argv) {
	
    artemis::World world;
    artemis::SystemManager * sm = world.getSystemManager();
    artemis::EntityManager * em = world.getEntityManager();

	sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
	UVSphericalRenderSystem * sphereRenderSys = (UVSphericalRenderSystem*)sm->setSystem(new UVSphericalRenderSystem(window));
	PlayerSystem * playerSys = (PlayerSystem*)sm->setSystem(new PlayerSystem());

    sm->initializeAll();

    for(int i = 0; i != 400; ++i) {
		float u = float(rand()%1000)/1000.0f, v = float(rand()%1000)/1000.0f;
		
		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("point.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("point2.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = 0.5+float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("point2.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = 0.5-float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("point2.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = 1-float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new UVPositionComponent(u, v));
		player.addComponent(new SpriteComponent("point2.png"));
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

		window.clear(sf::Color::Black);
		// draw everything here...
		sphereRenderSys->process();

		window.display();
	}

    return 0;
}