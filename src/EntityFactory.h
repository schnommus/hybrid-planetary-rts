#pragma once

#include <vector>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <Artemis/Artemis.h>
#include <tinyxml2.h>

#include "ProcessingSystem.h"

class AttributeList {
public:
	std::map< std::string, std::string > attributeMap;
	std::string String( std::string key, std::string defaultvalue );
	int Int( std::string key, int defaultvalue );
	float Float( std::string key, float defaultvalue );
};

class ComponentDescriptor {
public:
	ComponentDescriptor( std::string typev, AttributeList &attrv ) : type(typev), attr(attrv) { }
	std::string type;
	AttributeList attr;
};

class EntityFactory: public artemis::ProcessingSystem {
public:
	EntityFactory( sf::RenderTarget &windowv );
	artemis::Entity *Create( std::string type );
	std::vector< std::string > GetAllTypes();
protected:
	virtual void initialize();
	virtual void doProcessing();
private:
	std::vector< std::string > allTypes;
	std::map< std::string, std::vector< ComponentDescriptor > > entityTypeMap;
	sf::RenderTarget &window;
};
