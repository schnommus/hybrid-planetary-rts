#include "EntityFactory.h"

#include <tinydir.h>
#include <string>
#include <sstream>
#include "Components.h"
#include "ResourceManager.h"

using namespace tinyxml2;

XMLEntityFactory::XMLEntityFactory( ) {}

void XMLEntityFactory::initialize() {
	auto files = ResourceManager::Inst().AllFilesOfExtension( "xml" );
	for( int i = 0; i != files.size(); ++i ) {
		XMLDocument doc;
		doc.LoadFile( files[i].c_str() );

		if( doc.ErrorID() != 0 )
			std::cout << "XML Parse Error! ID: " << doc.ErrorID() << std::endl;

		XMLElement* entity = doc.FirstChildElement();
		std::string typeName = entity->Attribute( "type_name" );
		allTypes.push_back(typeName);
		entityTypeMap[typeName] = std::vector< ComponentDescriptor > ();
		XMLElement* currentComponent = entity->FirstChildElement();
		while( currentComponent != nullptr ) {
			AttributeList attr;
			const XMLAttribute *currentAttribute = currentComponent->FirstAttribute();
			while( currentAttribute != nullptr ) {
				attr.attributeMap[currentAttribute->Name()] = currentAttribute->Value();
				currentAttribute = currentAttribute->Next();
			}
			ComponentDescriptor desc( currentComponent->Name(), attr );
			entityTypeMap[typeName].push_back(desc);
			currentComponent = currentComponent->NextSiblingElement();
		}
		AttributeList attr; attr.attributeMap["name"] = typeName;
		ComponentDescriptor name( "name_tag", attr );
		entityTypeMap[typeName].push_back(name);
	}
}

void XMLEntityFactory::doProcessing() {
}

artemis::Entity * XMLEntityFactory::Create( std::string type ) {
	try {
		std::vector< ComponentDescriptor > &components = entityTypeMap.at(type);
		artemis::Entity* ent = &world->createEntity();
		for( int i = 0; i != components.size(); ++i ) {
			ent->addComponent( ComponentFromDescriptor( components[i] ) );
		}
		ent->refresh();
		return ent;
	} catch (...) {
		std::cout << "Attempted to create entity of unknown type: " << type;
		return nullptr;
	}
}

std::vector< std::string > XMLEntityFactory::GetAllTypes() {
	return allTypes;
}


std::string AttributeList::String( std::string key, std::string defaultvalue ) {
	try {
		return attributeMap.at(key);
	} catch (...) {
		return defaultvalue;
	}
}

int AttributeList::Int( std::string key, int defaultvalue ) {
	try {
		std::string val = attributeMap.at(key);
		std::istringstream iss( val );
		int result;
		iss >> result;
		if( iss.rdbuf()->in_avail() > 0 ) {
			std::cout << "Non-integer attribute parsed as integer. Using default for " << key << std::endl;
			throw std::exception();
		}
		return result;
	} catch (...) {
		return defaultvalue;
	}
}

float AttributeList::Float( std::string key, float defaultvalue ) {
	try {
		std::string val = attributeMap.at(key);
		std::istringstream iss( val );
		float result;
		iss >> result;
		if( iss.rdbuf()->in_avail() > 0 ) {
			std::cout << "Non-float attribute parsed as float. Using default for " << key << std::endl;
			throw std::exception();
		}
		return result;
	} catch (...) {
		return defaultvalue;
	}
}