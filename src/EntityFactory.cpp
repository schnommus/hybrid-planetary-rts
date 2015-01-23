#include "EntityFactory.h"

#include <tinydir.h>
#include <tinyxml2.h>
#include <string>

using namespace tinyxml2;

EntityFactory::EntityFactory( sf::RenderTarget &windowv )
	: window( windowv ) { }

void EntityFactory::initialize() {
	auto files = AllFilesOfExtension( "xml" );
	for( int i = 0; i != files.size(); ++i ) {
		XMLDocument doc;
		doc.LoadFile( files[i].c_str() );

		if( doc.ErrorID() != 0 )
			std::cout << "Error ID: " << doc.ErrorID() << std::endl;

		XMLElement* entity = doc.FirstChildElement();
		std::cout << "Entity type name: " << entity->Attribute( "type-name" ) << std::endl;
		std::cout << "Components: " << std::endl;
		XMLElement* currentComponent = entity->FirstChildElement();
		while( currentComponent != nullptr ) {
			std::cout << "\t" << currentComponent->Name() << " with" << std::endl;
			const XMLAttribute *currentAttribute = currentComponent->FirstAttribute();
			while( currentAttribute != nullptr ) {
				std::cout << "\t\t" << currentAttribute->Name() << "=" << currentAttribute->Value() << std::endl;
				currentAttribute = currentAttribute->Next();
			}
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
