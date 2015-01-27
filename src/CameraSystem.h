#pragma once

#include "ProcessingSystem.h"
#include "Game.h"
#include <../gamemath/Matrix4x3.h>

class CameraSystem : public artemis::ProcessingSystem {
public:
	float dtheta;
	float dgamma;
	Matrix4x3 worldtransform;
	Matrix4x3 sun;
	CameraSystem(Game &gamev) : game(gamev) {}
protected:
	virtual void doProcessing ();
	virtual void initialize ();
private:
	Game &game;
	float otheta;
	float ogamma;
};
