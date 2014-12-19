#include <Artemis\Artemis.h>
#include <SFML\Graphics.hpp>
#include <iostream>

#include "VecX.h"

class VelocityComponent : public artemis::Component {
public:
    float velocityX;
    float velocityY;

    VelocityComponent(float velocityX, float velocityY) {
        this->velocityX = velocityX;
        this->velocityY = velocityY;
    };
};

class PositionComponent : public artemis::Component {
public:
    float posX;
    float posY;
	float posZ;
	float u, v;

    PositionComponent(float posX, float posY) {
        this->u = posX;
        this->v = posY;
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

class MovementSystem : public artemis::EntityProcessingSystem {
private:
    artemis::ComponentMapper<VelocityComponent> velocityMapper;
    artemis::ComponentMapper<PositionComponent> positionMapper;

public:
	float theta, phi, gamma;

    MovementSystem() {
        addComponentType<VelocityComponent>();
        addComponentType<PositionComponent>();
    };

    virtual void initialize() {
        velocityMapper.init(*world);
        positionMapper.init(*world);
		theta = phi = gamma = 0.0f;
    };

    virtual void processEntity(artemis::Entity &e) {
		float &u = positionMapper.get(e)->u;
		float &v = positionMapper.get(e)->v;

		float x = cos(3.14159*u) * sin(3.14159*-2*v) * 200;
		float y = sin(3.14159*u) * sin(3.14159*-2*v) * 200;
		float z = cos(3.14159*-2*v) * 200;

		t3::Vec3<float> rotated = t3::Vec3<float>(x, y, z).AngularTransform( t3::Vec3<float>(theta, phi, gamma) );

		positionMapper.get(e)->posX = rotated.x+200;
		positionMapper.get(e)->posY = rotated.y+200;
		positionMapper.get(e)->posZ = rotated.z;

		//v += world->getDelta()/2;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			gamma += world->getDelta()/100;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			gamma -= world->getDelta()/100;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			theta += world->getDelta()/100;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			theta -= world->getDelta()/100;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::O))
			phi += world->getDelta()/100;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::L))
			phi -= world->getDelta()/100;

		//std::cout << theta << std::endl;

    };

};

class RenderSystem : public artemis::EntityProcessingSystem {
private:
	artemis::ComponentMapper<PositionComponent> positionMapper;
	artemis::ComponentMapper<SpriteComponent> spriteMapper;
	sf::RenderWindow &window;

public:
	RenderSystem( sf::RenderWindow &rwindow ) : window(rwindow) {
		addComponentType<PositionComponent>();
		addComponentType<SpriteComponent>();
	}

	virtual void initialize() {
		positionMapper.init(*world);
		spriteMapper.init(*world);
	};

	virtual void processEntity(artemis::Entity &e) {
		sf::Vector2f pos(positionMapper.get(e)->posX, positionMapper.get(e)->posY);
		sf::Sprite &s = spriteMapper.get(e)->sprite;
		s.setPosition( pos);
		if( positionMapper.get(e)->posZ < 0.0f ) {
			window.draw( s);
		}
	};
};

int main(int argc, char **argv) {
	
    artemis::World world;
    artemis::SystemManager * sm = world.getSystemManager();
    MovementSystem * movementsys = (MovementSystem*)sm->setSystem(new MovementSystem());
    artemis::EntityManager * em = world.getEntityManager();

	sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
	RenderSystem * rendersys = (RenderSystem*)sm->setSystem(new RenderSystem(window));

    sm->initializeAll();

    for(int i = 0; i != 400; ++i) {
		float u = float(rand()%1000)/1000.0f, v = float(rand()%1000)/1000.0f;
		
		artemis::Entity & player = em->create();
		player.addComponent(new VelocityComponent(80,40));
		player.addComponent(new PositionComponent(u, v));
		player.addComponent(new SpriteComponent("point.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new VelocityComponent(80,40));
		player.addComponent(new PositionComponent(u, v));
		player.addComponent(new SpriteComponent("point2.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = 0.5+float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new VelocityComponent(80,40));
		player.addComponent(new PositionComponent(u, v));
		player.addComponent(new SpriteComponent("point2.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = 0.5-float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new VelocityComponent(80,40));
		player.addComponent(new PositionComponent(u, v));
		player.addComponent(new SpriteComponent("point2.png"));
		player.refresh();
	}

	for(int i = 0; i != 50; ++i) {
		float u = float(rand()%1000)/1000.0f, v = 1-float(rand()%100)/1000.0f;

		artemis::Entity & player = em->create();
		player.addComponent(new VelocityComponent(80,40));
		player.addComponent(new PositionComponent(u, v));
		player.addComponent(new SpriteComponent("point2.png"));
		player.refresh();
	}


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
		movementsys->process();

		window.clear(sf::Color::Black);
		// draw everything here...
		rendersys->process();

		window.display();
	}

    return 0;
}