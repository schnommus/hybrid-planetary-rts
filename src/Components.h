#pragma once

#include <Artemis/Artemis.h>
#include <SFML/Graphics.hpp>

class AttributeList;
class ComponentDescriptor;

struct Tag : public artemis::Component {};

template <typename T>
inline T &FetchComponent( artemis::Entity &e ) {
	return *((T*)e.getComponent<T>());
}

class UVPositionComponent : public artemis::Component {
public:
	float u;
	float v;
	float screen_x;
	float screen_y;
	bool on_screen;
	sf::Color colour;
	UVPositionComponent (float uv=0, float vv=0);
	UVPositionComponent *From( artemis::Entity *e );
};


class FlatPositionComponent : public artemis::Component {
public:
	float x;
	float y;
	FlatPositionComponent (float xv, float yv);
	static artemis::Component *CreateFromAttributes( AttributeList &att );
};


class SpriteComponent : public artemis::Component {
public:
	sf::Sprite sprite;
	sf::Clock frameClock;
	int nFrames;
	SpriteComponent (std::string name, float scale = 1.0f, int frames = 1);
	void UpdateAnimation ();
	static artemis::Component *CreateFromAttributes( AttributeList &att );
private:
	sf::Texture texture;
};

class UITag : public Tag { };

class TerrainNodeTag : public Tag { };

class MinimapTag : public Tag { };

class BackgroundTerrainTag : public Tag { };

class StarTag : public Tag { };


artemis::Component *ComponentFromDescriptor( ComponentDescriptor & desc );