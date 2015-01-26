#include "SelectionSystem.h"

#include <string>
#include <sstream>
#include "Components.h"
#include "ResourceManager.h"


SelectionSystem::SelectionSystem(Game &gamev) : game(gamev) {

}

void SelectionSystem::initialize() {
	desc.setFont( ResourceManager::Inst().GetFont("RiskofRainFont.ttf") );
	desc.setCharacterSize(8);
	loopSelecting = false;
}

void SelectionSystem::doProcessing() {
	if( sf::Mouse::isButtonPressed( sf::Mouse::Left) ) {
		if( loopSelecting == false ) {
			loopSelecting = true;
			boxOrigin.x = game.LocalMousePosition().x;
			boxOrigin.y = game.LocalMousePosition().y;
		}
	} else {
		loopSelecting = false;
	}

	if( sf::Mouse::isButtonPressed( sf::Mouse::Left) ) {
		if( ! sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ) selectedEntities.clear();
		sf::Vector2i mpos = game.LocalMousePosition();
		for( int i = 0; i != world->getEntityManager()->getTotalCreated(); ++i ) {
			if( &world->getEntity(i) != nullptr ) {
				if( &FetchComponent<SelectableComponent>(world->getEntity(i)) != nullptr ) {
					UVPositionComponent &pos = FetchComponent<UVPositionComponent>(world->getEntity(i));
					if( pos.on_screen ) {
						if( sqrt( (pos.screen_x-mpos.x)*(pos.screen_x-mpos.x) + (pos.screen_y-mpos.y)*(pos.screen_y-mpos.y) ) < 10 ) {
							FetchComponent<SelectableComponent>(world->getEntity(i)).isSelected = true;
							selectedEntities.push_back(&world->getEntity(i));
						} else if (loopSelecting && isInsideRectangle( pos.screen_x, pos.screen_y, boxOrigin.x, boxOrigin.y, game.LocalMousePosition().x, game.LocalMousePosition().y )) {
							FetchComponent<SelectableComponent>(world->getEntity(i)).isSelected = true;
							selectedEntities.push_back(&world->getEntity(i));
						} else {
							FetchComponent<SelectableComponent>(world->getEntity(i)).isSelected = false;
						}
					}
				}
			}
		}
	}
}

void SelectionSystem::drawUnder() {
	sf::Sprite selectionCircle( ResourceManager::Inst().GetTexture("SelectionCircle.png") );
	selectionCircle.setOrigin( selectionCircle.getLocalBounds().width/2, selectionCircle.getLocalBounds().height/2 );
	for( int i = 0; i != selectedEntities.size(); ++i ) {
		UVPositionComponent &pos = FetchComponent<UVPositionComponent>(*selectedEntities[i]);
		if( !pos.on_screen ) {
			selectionCircle.setColor(sf::Color(190, 190, 190, 128));
		}
		selectionCircle.setPosition(pos.screen_x, pos.screen_y);
		game.Renderer()->draw(selectionCircle);
	}
}

void SelectionSystem::drawOver() {
	if( loopSelecting ) {
		sf::VertexArray a;
		a.setPrimitiveType(sf::LinesStrip);
		a.append( sf::Vertex( sf::Vector2f( boxOrigin.x, boxOrigin.y ), sf::Color::White));
		a.append( sf::Vertex( sf::Vector2f( game.LocalMousePosition().x, boxOrigin.y ), sf::Color::White));
		a.append( sf::Vertex( sf::Vector2f( game.LocalMousePosition().x, game.LocalMousePosition().y ), sf::Color::White));
		a.append( sf::Vertex( sf::Vector2f( boxOrigin.x, game.LocalMousePosition().y ), sf::Color::White));
		a.append( sf::Vertex( sf::Vector2f( boxOrigin.x, boxOrigin.y ), sf::Color::White));

		game.Renderer()->draw(a);
	}

	if( selectedEntities.empty() ) {
		desc.setString("Nothing selected");
	} else if ( selectedEntities.size() == 1 ) {
		desc.setString(FetchComponent<SelectableComponent>(*selectedEntities[0]).description);
	} else {
		std::ostringstream oss;
		oss << selectedEntities.size() << " selected";
		desc.setString(oss.str());
	}

	desc.setPosition(168, 227);
	game.Renderer()->draw(desc);
}

bool SelectionSystem::isInsideRectangle( int x, int y, int x1, int y1, int x2, int y2 ) {
	if( y1 < y2 && x1 < x2 && x > x1 && x < x2 && y > y1 && y < y2) return true;
	if( y1 > y2 && x1 > x2 && x < x1 && x > x2 && y < y1 && y > y2) return true;
	if( y1 < y2 && x1 > x2 && x < x1 && x > x2 && y > y1 && y < y2) return true;
	if( y1 > y2 && x1 < x2 && x > x1 && x < x2 && y < y1 && y > y2) return true;
	return false;
}
