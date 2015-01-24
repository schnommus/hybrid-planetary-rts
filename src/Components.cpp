#include "Components.h"
#include "EntityFactory.h"
#include "ResourceManager.h"

#define register_component(s, t) if( desc.type == s ) return t::CreateFromAttributes( desc.attr )
#define register_tag(s, t) if( desc.type == s ) return new t()
artemis::Component *ComponentFromDescriptor( ComponentDescriptor & desc ) {
	register_component("sprite_component", SpriteComponent);
	register_tag("minimap_tag", MinimapTag);
	register_tag("terrain_node_tag", TerrainNodeTag);
	register_tag("uv_position_component", UVPositionComponent);
}

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


SpriteComponent::SpriteComponent (std::string name, float scale, int frames)
	: nFrames (frames) {
	sprite.setTexture( ResourceManager::Inst().GetTexture(name) );
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

artemis::Component *SpriteComponent::CreateFromAttributes( AttributeList &att ) {
	return new SpriteComponent( att.String("file_name", "point.png"), att.Float("scale", 1.0f), att.Int("animation_frames", 1));
}