#include "ResourceManager.h"

#include <tinydir.h>
#include <string>
#include <sstream>
#include "Components.h"

using namespace tinyxml2;

ResourceManager * ResourceManager::_inst = 0;
ResourceManager &ResourceManager::Inst() {
	if(!_inst)
		_inst = new ResourceManager();
	return *_inst;
}

const sf::Texture &ResourceManager::GetTexture( std::string name ) {
	try {
		return textureMap.at(name);
	} catch( ... ) {
		sf::Texture tex;
		tex.loadFromFile(textureDirectory + name);
		textureMap[name] = tex;
		return textureMap[name];
	}
}

const sf::Font & ResourceManager::GetFont( std::string name ) {
	try {
		return fontMap.at(name);
	} catch( ... ) {
		sf::Font font;
		font.loadFromFile(fontDirectory + name);
		fontMap[name] = font;
		return fontMap[name];
	}
}

std::vector<std::string> ResourceManager::AllFilesOfExtension( std::string typeToFind ) {
	std::vector<std::string> result;

	tinydir_dir dir;
	tinydir_open(&dir, xmlDirectory.c_str());

	while (dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (!file.is_dir) {
			std::string fileName( file.name );
			if( fileName.size() > 3 )
				if( fileName.substr(fileName.size()-3, fileName.size()-1 ) == typeToFind )
					result.push_back(xmlDirectory+fileName);
		}
		tinydir_next(&dir);
	}

	tinydir_close(&dir);

	return result;
}


ResourceManager::ResourceManager() {
	// Protip: Be sure to add the slash at the end
	textureDirectory = "../media/textures/";
	xmlDirectory = "../media/xml/";
	fontDirectory = "../media/fonts/";
}
