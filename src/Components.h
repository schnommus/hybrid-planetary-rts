#pragma once

#include <Artemis/Artemis.h>
#include <SFML/Graphics.hpp>

class UVPositionComponent : public artemis::Component {
public:
	float u;
	float v;
	float screen_x;
	float screen_y;
	bool on_screen;
	sf::Color colour;
	UVPositionComponent (float uv, float vv);
};


class FlatPositionComponent : public artemis::Component {
public:
	float x;
	float y;
	FlatPositionComponent (float xv, float yv);
};


class SpriteComponent : public artemis::Component {
public:
	sf::Sprite sprite;
	sf::Clock frameClock;
	int nFrames;
	SpriteComponent (std::string directory, float scale = 1.0f, int frames = 1);
	void UpdateAnimation ();
private:
	sf::Texture texture;
};


class TerrainNodeComponent : public artemis::Component {
public:
	std::string type;
	TerrainNodeComponent (std::string typev);
};


class MinimapComponent : public artemis::Component {
public:
	MinimapComponent ();
};


class BackgroundTerrainComponent : public artemis::Component {
public:
	BackgroundTerrainComponent ();
};


class StarComponent : public artemis::Component {
public:
	StarComponent ();
};