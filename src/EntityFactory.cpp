#include "EntityFactory.h"

#include <tinydir.h>
#include <string>
#include <sstream>
#include "Components.h"

using namespace tinyxml2;

EntityFactory::EntityFactory( sf::RenderTarget &windowv )
	: window( windowv ) { }

void EntityFactory::initialize() {
	auto files = AllFilesOfExtension( "xml" );
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
	}
}

std::vector<std::string> EntityFactory::AllFilesOfExtension( std::string typeToFind ) {
	std::vector<std::string> result;

	tinydir_dir dir;
	tinydir_open(&dir, "../media");

	while (dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (!file.is_dir) {
			std::string fileName( file.name );
			if( fileName.size() > 3 )
				if( fileName.substr(fileName.size()-3, fileName.size()-1 ) == typeToFind )
					result.push_back("../media/"+fileName);
		}
		tinydir_next(&dir);
	}

	tinydir_close(&dir);

	return result;
}

void EntityFactory::doProcessing() {
}

artemis::Entity * EntityFactory::Create( std::string type ) {
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

std::vector< std::string > EntityFactory::GetAllTypes() {
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