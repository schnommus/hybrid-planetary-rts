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
	int offset_x, offset_y;
	float animation_speed;
	SpriteComponent (std::string name, float scale = 1.0f, int frames = 1, int offset_xv=0, int offset_yv=0, float animation_speedv=1.0f);
	void UpdateAnimation ();
	static artemis::Component *CreateFromAttributes( AttributeList &att );
	sf::Texture texture;
};

class NameComponent : public artemis::Component  {
	public:
		NameComponent( std::string namev ) : name(namev) {}
		std::string name;
		static artemis::Component *CreateFromAttributes( AttributeList &att );
};

class SelectableComponent : public artemis::Component {
public:
	SelectableComponent( std::string descriptionv ) : description(descriptionv) { }
	static artemis::Component *CreateFromAttributes( AttributeList &att );
	std::string description;
	bool isSelected;
};

class TransformOnBiome : public artemis::Component {
public:
	TransformOnBiome( std::string typev, std::string entityv );
	static artemis::Component *CreateFromAttributes( AttributeList &att );
	std::vector<std::string> types, newentities;
};

class UITag : public Tag { };

class TerrainNodeTag : public Tag { };

class MinimapTag : public Tag { };

class BackgroundTerrainTag : public Tag { 
public:
	int parentId;
	sf::Sprite sprite;
};

class StarTag : public Tag { };


artemis::Component *ComponentFromDescriptor( ComponentDescriptor & desc );