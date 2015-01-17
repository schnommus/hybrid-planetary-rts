#pragma once

#include "ProcessingSystem.h"
#include <..\gamemath\Matrix4x3.h>

class CameraSystem : public artemis::ProcessingSystem {
public:
	float dtheta;
	float dgamma;
	Matrix4x3 worldtransform;
	Matrix4x3 sun;
protected:
	virtual void doProcessing ();
	virtual void initialize ();
private:
	float otheta;
	float ogamma;
};