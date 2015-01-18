#include "Components.h"


UVPositionComponent::UVPositionComponent (float uv, float vv) {
	this->u = uv;
	this->v = vv;
	screen_x = screen_y = 0.0f;
	colour = sf::Color(255, 255, 255, 255);
	on_screen = false;
}


FlatPositionComponent::FlatPositionComponent (float xv, float yv) {
	this->x = xv;
	this->y = yv;
}


SpriteComponent::SpriteComponent (std::string directory, float scale, int frames)
	: nFrames (frames) {
	texture.loadFromFile("../media/" + directory);
	sprite.setTexture(texture);
	sprite.setTextureRect( sf::IntRect((rand()%nFrames)*sprite.getLocalBounds().width/frames, 0, sprite.getLocalBounds().width/frames, sprite.getLocalBounds().height ) );
	sprite.setScale(scale, scale);
	sprite.setOrigin( sprite.getLocalBounds().width/2, sprite.getLocalBounds().height/2 );
}


void SpriteComponent::UpdateAnimation () {
	if(  nFrames != 1 && frameClock.getElapsedTime().asSeconds() > 0.1f ) {
		frameClock.restart();

		int x = sprite.getTextureRect().left;
		x += sprite.getLocalBounds().width;
		if( x >= sprite.getTexture()->getSize().x )
			x = 0;

		sprite.setTextureRect( sf::IntRect(x, 0, sprite.getLocalBounds().width, sprite.getLocalBounds().height ) );
	}
}
TerrainNodeComponent::TerrainNodeComponent (std::string typev)
	: type (typev)
{}


MinimapComponent::MinimapComponent () {}
BackgroundTerrainComponent::BackgroundTerrainComponent () {}
StarComponent::StarComponent () {}