#include "Body.h"
Body::Body() : pos{}, vel{}, accel{}, angAcc{}, angVel {}, radius{1.0f}, mass{1.0f}, mesh{nullptr}, texture{nullptr} {
}

Body::~Body() {}


void Body::Update(float deltaTime) {
	/// From 1st semester physics class
	pos += vel * deltaTime + 0.5f * accel * deltaTime * deltaTime;
	vel += accel * deltaTime;
}

void Body::ApplyForce(Vec3 force) {
	accel = force / mass;
}

Matrix4 Body::GetModelMatrix() const
{
	// Model matrix sets the scale, rotation, and position of the mesh
	const Matrix4 translation = MMath::translate(pos);
	const Matrix4 rotation = MMath::toMatrix4(orientation);
	const Matrix4 scale = MMath::scale(size.x, size.y, size.z);
	return translation * rotation * scale;
}

void Body::UpdateAngularVel(float deltaTime){
	angVel += angAcc * deltaTime ;
}

void Body::ApplyTorque(Vec3 force){
					//make this 0.5 or 0.1 ^ ^
	float I = CalculateRotationalInertia();
	inertiaTensor = {
	 I,0.0f,0.0f,
	 0.0f,I,0.0f,
	 0.0f,0.0f,I
	};
	Vec3 torque = force;
	float test = CalculateRotationalInertia(); 
	if (test < VERY_SMALL) {
		//std::cout << test << " :RotationalInertia is to small" << std::endl;
		return;
	}
	angAcc = MMath::inverse(inertiaTensor) * torque;// - frictionTorque); // Use rotational inertia to update angular acceleration
	//std::cout << "Angular Acceleration: " << "x: " << angAcc.x  << ", y: " << angAcc.y << ", z: " << angAcc.z << std::endl;
}

float Body::CalculateRotationalInertia() const{
		return (2.0f / 5.0f) * mass * (radius * radius);  // Sphere moment of inertia
}

void Body::LinearVelocityToAngularVelocity(){
	if (VMath::mag(vel) < VERY_SMALL) return;
	const float magnitude = VMath::mag(vel) / 2.5f; // Radius to 2.5f;
	Vec3 cross = VMath::cross(Vec3(0.0f, 1.0f, 0.0f), vel);
	if (VMath::mag(cross) < VERY_SMALL) return;
	const Vec3 direction = VMath::normalize(cross);
	angVel = direction * magnitude;
}

Vec3 Body::ApplyFriction(){
	if (VMath::mag(vel) < VERY_SMALL) return Vec3{ 0, 0, 0 };
	const float normal_force = mass * 9.8f;
	constexpr float coefficient_of_friction = 0.1f;
	Vec3 force = coefficient_of_friction * normal_force * VMath::inverse(VMath::normalize(vel));

	return force;
	
}

void Body::updatePos(float deltaTime){
	pos += vel * deltaTime + 0.5f * accel * deltaTime * deltaTime; 
}

void Body::UpdateVel(float deltaTime){
	vel += accel * deltaTime;  
}

void Body::UpdateAngularAcc(float deltaTime){
	Vec3 frictionTorque = -dampingCoefficient * angVel;
		angAcc = frictionTorque / CalculateRotationalInertia();
}

void Body::RodConstraint(float deltaTime, Vec3 anchorPoint, float rodLength){
	Vec3 rodVector = anchorPoint - pos; // set it to the vector from body’s position to the anchor point
	float positionConstraint = VMath::mag(rodVector) - rodLength; // set to the magnitude of the rodVector minus the 
	if (VMath::mag(rodVector) < VERY_SMALL) { return; }
	float JV = VMath::dot(rodVector, vel) / MATH::VMath::mag(rodVector);
	const float baumgarteStabilizationParameter = 0.05; // Try tuning this number
	float b = -(baumgarteStabilizationParameter / deltaTime) * positionConstraint;
	float lambda = -mass * (JV + b);

	Vec3 Jtransposed = rodVector / VMath::mag(rodVector);
	Vec3 deltaVel = Jtransposed * lambda / mass;
	vel += deltaVel;
}

 void Body::LineCollision(Body* sphere, Body* pointOnPlane, Triangle* triangleShape, Body* collisionPoint, DualQuat line01, DualQuat line12, DualQuat line20,
	Body* pointOnLine01, Body* pointOnLine12, Body* pointOnLine20) {

	if (TMath::isPointInsideTriangle(pointOnPlane->pos, *triangleShape)) {
		collisionPoint->pos = pointOnPlane->pos;
	}
	else {
		// What are the two closest edges to the point on the plane?
		float dist01 = fabs(DQMath::orientedDist(pointOnPlane->pos, line01));
		float dist12 = fabs(DQMath::orientedDist(pointOnPlane->pos, line12));
		float dist20 = fabs(DQMath::orientedDist(pointOnPlane->pos, line20));
		if (dist01 <= dist12 && dist01 <= dist20) {
			// If line01 is the closest line, then use that projected position
			collisionPoint->pos = pointOnLine01->pos;
			// Hold on, what if that position is outide the triangle?
			if (!TMath::isPointInsideTriangle(collisionPoint->pos, *triangleShape)) {
				if (dist12 <= dist20) {
					collisionPoint->pos = pointOnLine12->pos;
				}
				else {
					collisionPoint->pos = pointOnLine20->pos;
				}
			}
		}
		else if (dist12 <= dist01 && dist12 <= dist20) {
			// In this case, line12 is the closest line
			collisionPoint->pos = pointOnLine12->pos;
			// But wait! What if that is not inside the triangle?
			if (!TMath::isPointInsideTriangle(collisionPoint->pos, *triangleShape)) {
				if (dist01 <= dist20) {
					collisionPoint->pos = pointOnLine01->pos;
				}
				else {
					collisionPoint->pos = pointOnLine20->pos;
				}
			}
		}
		else {
			// Final case is where line20 is the closest line
			collisionPoint->pos = pointOnLine20->pos;
			// Hold your horses! What if that is not inside the triangle?
			if (!TMath::isPointInsideTriangle(collisionPoint->pos, *triangleShape)) {
				if (dist01 <= dist12) {
					collisionPoint->pos = pointOnLine01->pos;
				}
				else {
					collisionPoint->pos = pointOnLine12->pos;
				}
			}
		}
	}
		// Now that you have the collision point, let's check if we will collide
// I'm tired after all this coding. Let's use Scott's distance method
	if (VMath::distance(sphere->pos, collisionPoint->pos) <= sphere->radius) {
		// Find the collision normal vector
		Vec3 collisionNormal = VMath::normalize(sphere->pos - collisionPoint->pos);
		// Reflect the sphere off the triangle
		// Scott to the rescue again with his reflect method
		if (VMath::mag(sphere->vel) > VERY_SMALL) {
			sphere->vel = VMath::reflect(sphere->vel, collisionNormal);
			
			
			float d = VMath::dot(collisionNormal, sphere->vel);
			
				
			sphere->pos += ((d - sphere->radius) * -VMath::normalize(collisionNormal)) / 50;
			//sphere->pos += VMath::normalize(sphere->vel) / 10;
		}
	}
}



void Body::UpdateOrientation(float deltaTime){
	float angleRadians = VMath::mag(angVel);
	// if angle is Zero, get outta here.
	if (angleRadians < VERY_SMALL) { // BEWARE!!! DONT USE == 0 FOR FLOATS
		//std::cout << "Angular velocity is too small, no rotation will be applied." << std::endl;
		return;
	}
	Vec3 axis = VMath::normalize(angVel);
	
	Quaternion rotation = QMath::angleAxisRotation((angleRadians * RADIANS_TO_DEGREES) * deltaTime, axis);
	orientation = rotation * orientation;
	QMath::normalize(orientation);
}

bool Body::OnCreate() {
	return true;
}

void Body::OnDestroy() {
}

void Body::Render() const {
}

