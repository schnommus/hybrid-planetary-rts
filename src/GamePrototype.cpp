#include <Artemis\Artemis.h>
#include <SFML\Graphics.hpp>
#include <iostream>

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

    PositionComponent(float posX, float posY) {
        this->posX = posX;
        this->posY = posY;
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
    MovementSystem() {
        addComponentType<VelocityComponent>();
        addComponentType<PositionComponent>();
    };

    virtual void initialize() {
        velocityMapper.init(*world);
        positionMapper.init(*world);
    };

    virtual void processEntity(artemis::Entity &e) {
        positionMapper.get(e)->posX += velocityMapper.get(e)->velocityX * world->getDelta();
        positionMapper.get(e)->posY += velocityMapper.get(e)->velocityY * world->getDelta();
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
		window.draw( s);
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

    artemis::Entity & player = em->create();

    player.addComponent(new VelocityComponent(80,40));
    player.addComponent(new PositionComponent(0,0));
	player.addComponent(new SpriteComponent("placeholder.png"));
    player.refresh();

    PositionComponent * comp = (PositionComponent*)player.getComponent<PositionComponent>();

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