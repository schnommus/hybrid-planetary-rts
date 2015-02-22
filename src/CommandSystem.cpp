#include "CommandSystem.h"

#include <string>
#include <sstream>
#include <cmath>
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


double wrap_pi( double angle ) {
	angle += M_PI;
	double twoPi = 2.0 * M_PI;
	return angle - twoPi * floor( angle / twoPi ) - M_PI;
}

float lerpAngle(float u, float v, float p) {
	return wrap_pi(u) + p*wrap_pi(wrap_pi(v) - wrap_pi(u));
}

double nth_root(double x, int n){
	if (!(n%2) || x<0){
		std::cout << "Bad Nth Root!";
	}

	bool sign = (x >= 0);

	x = exp(log(abs(x))/n);

	return sign ? x : -x;
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
					if( m->lerpDelta > 0.999f ) {
						m->isMoving = false;
					}
					UVPositionComponent &myPos = FetchComponent<UVPositionComponent>(world->getEntity(i));
					
					sf::Vector2f uv = m->target;
					
					float accelDelta = m->lerpDelta;

					myPos.u = lerpAngle( M_PI*m->startingPosition.x, M_PI*uv.x, accelDelta )/M_PI;
					myPos.v = lerpAngle( M_PI*m->startingPosition.y, M_PI*uv.y, accelDelta )/M_PI;


					float d_u = abs( lerpAngle( M_PI*m->startingPosition.x, M_PI*uv.x, 0.01f )/M_PI-lerpAngle( M_PI*m->startingPosition.x, M_PI*uv.x, 0.0f )/M_PI );
					float d_v = abs( lerpAngle( M_PI*m->startingPosition.y, M_PI*uv.y, 0.01f )/M_PI-lerpAngle( M_PI*m->startingPosition.y, M_PI*uv.y, 0.0f )/M_PI );
					float d_uv = 1.0f/sqrt( d_u*d_u + d_v*d_v );
					m->lerpDelta += world->getDelta()*d_uv/2000.0f*m->speed;
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
			UVPositionComponent &myPos = FetchComponent<UVPositionComponent>(*selected[i]);
			FetchComponent<MoveComponent>(*selected[i]).Initiate( sf::Vector2f( uv.x, uv.y ), sf::Vector2f(myPos.u, myPos.v) );

			++numTargeted;
			if( numTargeted == 5) {
				numTargeted = 0;
				++newRow;
			}
		}
	}
}
