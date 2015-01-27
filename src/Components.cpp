#include "Components.h"
#include "EntityFactory.h"
#include "ResourceManager.h"

#include <sstream>
#include <iostream>

#define register_component(s, t) if( desc.type == s ) return t::CreateFromAttributes( desc.attr )
#define register_tag(s, t) if( desc.type == s ) return new t()
artemis::Component *ComponentFromDescriptor( ComponentDescriptor & desc ) {
	register_component("sprite_component", SpriteComponent);
	register_component("flat_position_component", FlatPositionComponent);
	register_component("name_tag", NameComponent);
	register_component("selectable_component", SelectableComponent);
	register_component("transform_on_biome", TransformOnBiome);
	register_tag("ui_tag", UITag);
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

artemis::Component * FlatPositionComponent::CreateFromAttributes( AttributeList &att ) {
	return new FlatPositionComponent( att.Float("x", 0.0), att.Float("y", 0.0));
}


SpriteComponent::SpriteComponent (std::string name, float scale, int frames, int offset_xv, int offset_yv)
	: nFrames (frames), offset_x(offset_xv), offset_y(offset_yv) {
	sprite.setTexture( ResourceManager::Inst().GetTexture(name) );
	sprite.setTextureRect( sf::IntRect((rand()%nFrames)*sprite.getLocalBounds().width/frames, 0, sprite.getLocalBounds().width/frames, sprite.getLocalBounds().height ) );
	sprite.setScale(scale, scale);
	sprite.setOrigin( sprite.getLocalBounds().width/2+offset_x, sprite.getLocalBounds().height/2+offset_y );
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
	return new SpriteComponent( att.String("file_name", "point.png"), att.Float("scale", 1.0f), att.Int("animation_frames", 1), att.Int("offset_x", 0), att.Int("offset_y", 0));
}

artemis::Component * NameComponent::CreateFromAttributes( AttributeList &att ) {
	return new NameComponent( att.String("name", "unknown"));
}

artemis::Component * SelectableComponent::CreateFromAttributes( AttributeList &att ) {
	return new SelectableComponent( att.String("description", "Unknown Object") );
}

artemis::Component * TransformOnBiome::CreateFromAttributes( AttributeList &att ) {
	return new TransformOnBiome( att.String("type", "notspecified"), att.String("newentity", "notspecified") );
}

TransformOnBiome::TransformOnBiome( std::string typev, std::string entityv ) {
	typev.erase(std::remove_if(typev.begin(), typev.end(), ::isspace), typev.end());
	entityv.erase(std::remove_if(entityv.begin(), entityv.end(), ::isspace), entityv.end());

	std::stringstream in1(typev);
	std::string segment;
	while(std::getline(in1, segment, ',')) {
		types.push_back(segment);
	}

	std::stringstream in2(entityv);
	while(std::getline(in2, segment, ',')) {
		newentities.push_back(segment);
	}
}
