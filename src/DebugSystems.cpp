#include "DebugSystems.h"
#include "ResourceManager.h"
#include "CameraSystem.h"
#include "RenderSystem.h"
#include <iomanip>
#include <../gamemath/vector3.h>
#include <sstream>

DrawDebugInfoSystem::DrawDebugInfoSystem (Game &gamev)
	: game(gamev) {}

void DrawDebugInfoSystem::doProcessing () {
	std::ostringstream oss;
	oss << int(1.0f/world->getDelta()) << " FPS";
	fps_text.setString(oss.str());
	fps_text.setPosition(0, 0);
	game.Renderer()->draw(fps_text);

	// Draw latitude; longitude
	oss.str( std::string() );
	float sz = 500.0f;
	sf::Vector2f out = ReverseUVTransform( Vector3( 0, 0, sz ), sz, game.Camera()->worldtransform);
	oss << std::setfill('0') << std::setw(2) << (int)(abs(out.y*2.0f*180.0f-90.0f)) << "*" << (out.y<0.25f? "N" : "S") << " : ";
	oss << std::setfill('0') << std::setw(3) << (int)(abs(out.x*180.0f)) << "*" << (out.x>0.0f? "E" : "W");
	fps_text.setString(oss.str());
	fps_text.setPosition(68, 227);
	game.Renderer()->draw(fps_text);
}

void DrawDebugInfoSystem::initialize () {
	fps_text.setFont( ResourceManager::Inst().GetFont("RiskofRainFont.ttf") );
	fps_text.setCharacterSize(8);
}