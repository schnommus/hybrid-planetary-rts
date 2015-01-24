#pragma once

#include "ProcessingSystem.h"

#include <SFML/Graphics.hpp>

class DrawFPSSystem : public artemis::ProcessingSystem {
public:
	DrawFPSSystem (sf::RenderTarget & rwindow);
protected:
	virtual void doProcessing ();
	virtual void initialize ();
private:
	sf::Text fps_text;
	sf::RenderTarget & window;
};