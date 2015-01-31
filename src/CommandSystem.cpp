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
					if( abs( FetchComponent<UVPositionComponent>(world->getEntity(i)).u-m->target.x ) < 0.003f && abs( FetchComponent<UVPositionComponent>(world->getEntity(i)).v-m->target.y ) < 0.003f ) {
						m->isMoving = false;
					}
					UVPositionComponent &myPos = FetchComponent<UVPositionComponent>(world->getEntity(i));
					
					sf::Vector2f uv = m->target;
					float d = abs(uv.x-myPos.u); //conventional
					float d2;
					if( uv.x < myPos.u ) {
						d2 = abs( 2+uv.x-myPos.u ); d2 = (d2>2.0f?d2-2.0f:d2); //wrapped
					} else {
						d2 = abs( 2+myPos.u-uv.x ); d2 = (d2>2.0f?d2-2.0f:d2); //wrapped
					}
					if( d2 < d ) {
						float theta = atan2( (uv.y-myPos.v), (uv.x-myPos.u) );
						m->velocity.x = -cos(theta);
						m->velocity.y = sin(theta);
					} else {
						float theta = atan2( (uv.y-myPos.v), (uv.x-myPos.u) );
						m->velocity.x = cos(theta);
						m->velocity.y = sin(theta);
					}

					if( myPos.u <= -1.0f) myPos.u = 0.999f;
					if( myPos.u >= 1.0f) myPos.u = -0.999f;
					myPos.u += 0.08*world->getDelta()*m->velocity.x;
					myPos.v += 0.08*world->getDelta()*m->velocity.y;
					myPos.u += abs( asin(4.0f*myPos.v-1.0f) )*0.30*world->getDelta()*m->velocity.x;
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
