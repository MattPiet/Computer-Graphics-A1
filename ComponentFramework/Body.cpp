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
	const float baumgarteStabilizationParameter = 0.18; // Try tuning this number
	float b = -(baumgarteStabilizationParameter / deltaTime) * positionConstraint;
	float lambda = -mass * (JV + b);

	Vec3 Jtransposed = rodVector / VMath::mag(rodVector);
	Vec3 deltaVel = Jtransposed * lambda / mass;
	vel += deltaVel;
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

