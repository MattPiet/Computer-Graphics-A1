#pragma once
#include <Vector.h> /// This is in GameDev
#include <VMath.h>
#include <MMath.h>
#include <QMath.h>
using namespace MATH;

class Plane {
public:
	Vec3 normal;
	float d = 0.0f;
};