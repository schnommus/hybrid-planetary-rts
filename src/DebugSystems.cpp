#include "DebugSystems.h"

#include <sstream>

DrawFPSSystem::DrawFPSSystem (sf::RenderTarget & rwindow)
	: window (rwindow) {}

void DrawFPSSystem::doProcessing () {
	std::ostringstream oss;
	oss << int(1.0f/world->getDelta()) << " FPS";
	fps_text.setString(oss.str());
	window.draw(fps_text);
}

void DrawFPSSystem::initialize () {
	debugfont.loadFromFile("..//media//RiskofRainFont.ttf");
	fps_text.setFont( debugfont );
	fps_text.setCharacterSize(8);
}