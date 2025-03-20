#pragma once
#include "Body.h"
#include <Vector.h> /// This is in GameDev
#include <VMath.h>
#include <MMath.h>
#include <QMath.h>
using namespace MATH;
class Collision{
public:

	// Checks if two spheres have collided and return true or false.
// We are assuming that all Body objects are spheres.
// I know it’s weird, we’ll fix it next course
// Notice the “const” keyword here.
// That is a promise to the compilar that we won’t change the Body objects.
// We are just detecting collisions after all
	static bool SphereSphereCollisionDetected(const Body* body1, const Body* body2);

	// If a collision has occurred, we need to immediately change the velocity
	// of both spheres.
	// We will definitely be changing the objects so no “const” keywords here
	static void SphereSphereCollisionResponse(Body* body1, Body* body2);
};

