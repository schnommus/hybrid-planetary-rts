#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <Artemis/Artemis.h>
//#include <TinyXML2/tinyxml2.h>

#include "Components.h"
#include "ProcessingSystem.h"

class EntityFactory: public artemis::ProcessingSystem {
public:
	EntityFactory( sf::RenderTarget &windowv );
protected:
	virtual void initialize();
	virtual void doProcessing();
private:
	std::vector<std::string> AllFilesOfExtension( std::string typeToFind );
	sf::RenderTarget &window;
};
