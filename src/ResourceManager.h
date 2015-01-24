#pragma once

#include <vector>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <Artemis/Artemis.h>
#include <tinyxml2.h>


class ResourceManager {
public:
	static ResourceManager &Inst();
	const sf::Texture &GetTexture( std::string name );
	const sf::Font &GetFont( std::string name );
	std::vector<std::string> AllFilesOfExtension( std::string typeToFind );
private:
	ResourceManager();
	std::map< std::string, sf::Texture > textureMap;
	std::map< std::string, sf::Font > fontMap;
	std::string textureDirectory;
	std::string xmlDirectory;
	std::string fontDirectory;
	static ResourceManager *_inst;
};