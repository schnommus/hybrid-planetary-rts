#include "CameraSystem.h"

#include <SFML/Graphics.hpp>
#include <../gamemath/EulerAngles.h>
#include <../gamemath/vector3.h>

void CameraSystem::doProcessing () {
	// Movement acc/decceleration
	dtheta *= 0.98;
	dgamma *= 0.98;
	
	// Disable movement acc/decceleration
	dtheta = 0;
	dgamma = 0;

	// Update camera
	float fact = 3*world->getDelta()/2.0f;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || game.LocalMousePosition().x > game.Renderer()->getSize().x-2)
		dgamma -= fact;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || game.LocalMousePosition().x < 2)
		dgamma += fact;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || game.LocalMousePosition().y < 2)
		dtheta += fact;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || game.LocalMousePosition().y > game.Renderer()->getSize().y-2)
		dtheta -= fact;

	// Update matrices
	Matrix4x3 m1;
	m1.setupRotate( 1, -dtheta );

	Matrix4x3 m2;
	m2.setupRotate( 2, dgamma );

	worldtransform = worldtransform * (m1*m2);

	m2.setupRotate( 2, 0.2*world->getDelta() );
	sun = sun * m2;
}

void CameraSystem::initialize () {
	dtheta=dgamma=0.1f;
	worldtransform.setupRotate(1, M_PI/2); // Camera starting angle
	sun.setupLocalToParent( Vector3(0, 0, 0), EulerAngles(0.3, 1.8, 0) );
}
