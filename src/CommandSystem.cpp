#include "CommandSystem.h"

#include <string>
#include <sstream>
#include "Components.h"
#include "ResourceManager.h"
#include "SelectionSystem.h"
#include "RenderSystem.h"
#include "CameraSystem.h"
#include <../gamemath/vector3.h>


CommandSystem::CommandSystem(Game &gamev) : game(gamev), rmbDown(false) {

}

void CommandSystem::initialize() {
}

void CommandSystem::doProcessing() {
	if( !rmbDown && sf::Mouse::isButtonPressed(sf::Mouse::Right) ) {
		rmbDown = true;
		PerformAction();
	} else if( !sf::Mouse::isButtonPressed(sf::Mouse::Right) ) {
		rmbDown = false;
	}

	for( int i = 0; i != world->getEntityManager()->getTotalCreated(); ++i ) {
		if( &world->getEntity(i) != nullptr ) {
			MoveComponent *m = &FetchComponent<MoveComponent>(world->getEntity(i));
			if( m != nullptr ) {
				if( m->isMoving ) {
					if( abs( FetchComponent<UVPositionComponent>(world->getEntity(i)).u-m->target.x ) < 0.001f && abs( FetchComponent<UVPositionComponent>(world->getEntity(i)).v-m->target.y ) ) {
						m->isMoving = false;
					}
					FetchComponent<UVPositionComponent>(world->getEntity(i)).u -= 3*world->getDelta()*(FetchComponent<UVPositionComponent>(world->getEntity(i)).u-m->target.x);
					FetchComponent<UVPositionComponent>(world->getEntity(i)).v -= 3*world->getDelta()*(FetchComponent<UVPositionComponent>(world->getEntity(i)).v-m->target.y);
				}
			}
		}
	}
}

void CommandSystem::PerformAction() {
	// Get mouse position in world
	sf::Vector2i pos = game.LocalMousePosition();
	float x = pos.x, y = pos.y, sz=500.0f;
	x -= game.Renderer()->getSize().x/2;
	y -= game.Renderer()->getSize().y/2;

	// Count entities to be moved
	auto &selected = game.Selection()->SelectedEntities();
	int totalMoving = 0;
	for( int i = 0; i != selected.size(); ++i ) {
		if( &FetchComponent<MoveComponent>(*selected[i]) != nullptr ) {
			++totalMoving;
		}
	}

	// Move them
	int numTargeted = 0; // These two variables for grid formation
	int newRow = 0;
	for( int i = 0; i != selected.size(); ++i ) {
		if( &FetchComponent<MoveComponent>(*selected[i]) != nullptr ) {
			float x2 = x+15*(numTargeted-(totalMoving>4?3:totalMoving/2));
			float y2 = y+ 15*(newRow-totalMoving/12);
			float z = sqrt( sz*sz-x*x-y*y );
			sf::Vector2f uv = ReverseUVTransform( Vector3(x2, y2, z), sz, game.Camera()->worldtransform );
			FetchComponent<MoveComponent>(*selected[i]).Initiate( sf::Vector2f( uv.x, uv.y ) );
			++numTargeted;
			if( numTargeted == 5) {
				numTargeted = 0;
				++newRow;
			}
		}
	}
}
