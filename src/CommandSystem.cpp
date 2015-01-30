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
	std::cout << "CommandSystem::PerformAction()!" << std::endl;
	auto &selected = game.Selection()->SelectedEntities();
	for( int i = 0; i != selected.size(); ++i ) {
		sf::Vector2i pos = game.LocalMousePosition();
		float x = pos.x, y = pos.y, sz=500.0f;
		x -= game.Renderer()->getSize().x/2;
		y -= game.Renderer()->getSize().y/2;
		float z = sqrt( sz*sz-x*x-y*y );
		sf::Vector2f uv = ReverseUVTransform( Vector3(x, y, z), sz, game.Camera()->worldtransform );
		if( &FetchComponent<MoveComponent>(*selected[i]) != nullptr ) {
			FetchComponent<MoveComponent>(*selected[i]).Initiate( uv );
		}
	}
}
