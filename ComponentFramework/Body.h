#ifndef BODY_H
#define BODY_H
#include <Vector.h> /// This is in GameDev
#include <VMath.h>
#include <MMath.h>
#include <QMath.h>
using namespace MATH; 

/// Just forward declair these classes so I can define a pointer to them
/// Used later in the course
class Mesh;
class Texture;

class Body {
public:
    Body();
    ~Body();
	
public: /// Physics stuff
	Vec3 pos;
	Vec3 vel;
	Vec3 accel;
	Vec3 size = Vec3(1.0f, 1.0f, 1.0f);
	Vec3 angVel; // in r/s
	Vec3 angAcc; // in r/s
	Quaternion orientation; // when in 3D we need quaternions to track rotations and a bunch of other stuff
	Matrix3 inertiaTensor;
	float mass = 1.0f;
	//float angle;
	float radius = 1.0f;
	const float dampingCoefficient = 0.01f;
private: /// Graphics stuff 
	Mesh *mesh;
	Texture *texture;
	
public:
	bool OnCreate();
	void OnDestroy();
	void Update(float deltaTime);
	void Render() const;
	void ApplyForce(Vec3 force);
	void setAccel(const Vec3 &accel_) { accel = accel_;}
	// Updates orientation quaternion using the angular velocity
	void UpdateOrientation(float deltaTime);
	Matrix4 GetModelMatrix() const;
	// Updates angular velocity using rotational equations of motion
	void UpdateAngularVel(float deltaTime);
	// Updates angular acceleration using torque and rotational inertia
	void ApplyTorque(Vec3 torque);
	float CalculateRotationalInertia() const;
	void LinearVelocityToAngularVelocity();
	Vec3 ApplyFriction();
	void updatePos(float deltaTime);
	void UpdateVel(float deltaTime);
	void UpdateAngularAcc(float deltaTime);
	void RodConstraint(float deltaTime, Vec3 anchorPoint, float rodLength);

};

#endif
