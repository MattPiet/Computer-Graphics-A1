#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene0p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"

#include "MemoryMonitor.h"


Scene0p::Scene0p() :sphere{nullptr}, shader{nullptr}, mesh{nullptr},
drawInWireMode{true} {
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0p::~Scene0p() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}



bool Scene0p::OnCreate() {
	Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);
	std::cout << "---------------------------------------------------\n";
	sphere = new Body();
	sphere->OnCreate();

	plane = new Body();
	plane->OnCreate();

	plane_2 = new Body();
	plane_2->OnCreate();

	plane_3 = new Body();
	plane_3->OnCreate();

	planeShape = new 	surface::Plane();
	planeShape->normal = Vec3(0.0f, 0.0f, -1.0f);

	planeShape_2 = new 	surface::Plane();
	planeShape_2->normal = Vec3(0.0f, 0.0f, -1.0f);

	planeShape_3 = new 	surface::Plane();
	planeShape_3->normal = Vec3(0.0f, 0.0f, -1.0f);

	lightPos = Vec3(10.0f, 0.0f, 0.0f);
	mesh = new Mesh("meshes/Sphere.obj");
	mesh->OnCreate();

	planeMesh = new Mesh("meshes/Plane.obj");
	planeMesh->OnCreate();

	planeMesh_2 = new Mesh("meshes/Plane.obj");
	planeMesh_2->OnCreate();

	planeMesh_3 = new Mesh("meshes/Plane.obj");
	planeMesh_3->OnCreate();

	
	texture = new Texture();
	texture->LoadImage("textures/evilEye.png");

	planeText = new Texture();
	planeText->LoadImage("textures/stone.png");

	planeText_2 = new Texture();
	planeText_2->LoadImage("textures/stone.png");

	planeText_3 = new Texture();
	planeText_3->LoadImage("textures/stone.png");

	camera = new Camera();
	camera->OnCreate();


	shader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}
	//							planeVert						planeFrag
	planeShader = new Shader("shaders/planeVert.glsl", "shaders/planeFrag.glsl");
	if (planeShader->OnCreate() == false) {
		std::cout << "planeShader failed ... we have a problem\n";
	}

	planeShader_2 = new Shader("shaders/planeVert.glsl", "shaders/planeFrag.glsl");
	if (planeShader_2->OnCreate() == false) {
		std::cout << "planeShader_2 failed ... we have a problem\n";
	}

	planeShader_3 = new Shader("shaders/planeVert.glsl", "shaders/planeFrag.glsl");
	if (planeShader_3->OnCreate() == false) {
		std::cout << "planeShader_3 failed ... we have a problem\n";
	}
													
	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
														// 0.0f, 0.0f, 0.0f
	camera->GetViewMatrix() = MMath::lookAt(Vec3(0.0f, -30.0f, 30.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	modelMatrix.loadIdentity();
	planeModelMatrix.loadIdentity();
	sphere->GetModelMatrix() = modelMatrix;
	rotation = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
	plane->orientation *= rotation;
	planeShape->normal = QMath::rotate(planeShape->normal, rotation);
	plane->size *= 2;
	plane1_rotationHolder = rotation;
	//std::cout << planeShape->normal.x << ", y: " << planeShape->normal.y << ", z: " << planeShape->normal.z << std::endl;

	rotationForPlane2 = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
	plane_2->orientation *= rotationForPlane2;
	planeShape_2->normal = QMath::rotate(planeShape_2->normal, rotationForPlane2);
	plane_2->size *= 2;
	plane2_rotationHolder = rotationForPlane2;

	rotationForPlane3 = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
	plane_3->orientation *= rotationForPlane3;
	planeShape_3->normal = QMath::rotate(planeShape_3->normal, rotationForPlane3);
	plane_3->size *= 2;
	plane3_rotationHolder = rotationForPlane3;

	plane->pos = Vec3(0.0f, 0.0f, 0.0f);
	plane_2->pos = Vec3(0.0f, 0.0f, -20.0f);
	plane_3->pos = Vec3(-20.0f, 0.0f, 0.0f);
	drawInWireMode = 0;

	lightPosition[0] = Vec3(-10.0f, 10.0f, -5.0f); // top left it is also ks/kd[0] so red
	lightPosition[1] = Vec3(-10.0f, -10.0f, -5.0f); // bottom left it is also ks/kd[1] so green
	lightPosition[2] = Vec3(10.0f, -10.0f, -5.0f); // bottom right it is also ks/kd[2] so blue
	lightPosition[3] = Vec3(10.0f, 10.0f, -5.0f); // top right it is also ks/kd[3] so orangeish yellow
	lightPosition[4] = Vec3(0.0f, -10.0f, -5.0f); // bottom it is also ks/kd[4] so a bit purple

	ks[0] = Vec4(0.4, 0.1, 0.1, 0.0);  // Soft red highlight
	ks[1] = Vec4(0.1, 0.4, 0.1, 0.0);  // Soft green highlight
	ks[2] = Vec4(0.1, 0.1, 0.4, 0.0);  // Soft blue highlight
	ks[3] = Vec4(0.4, 0.4, 0.1, 0.0);  // Warm yellow
	ks[4] = Vec4(0.8, 0.1, 0.8, 0.0);  // Magenta touch

	kd[0] = Vec4(0.7, 0.3, 0.3, 0.0);  // Soft red-pink
	kd[1] = Vec4(0.3, 0.7, 0.3, 0.0);  // Soft green
	kd[2] = Vec4(0.3, 0.3, 0.7, 0.0);  // Soft blue
	kd[3] = Vec4(0.8, 0.5, 0.2, 0.0);  // Subdued orange
	kd[4] = Vec4(0.8, 0.3, 0.8, 0.0);  // Gentle purple

	Plane_lightPosition[0] = Vec3(-10.0f, 10.0f, -5.0f); // top left it is also ks/kd[0] so red
	Plane_lightPosition[1] = Vec3(-10.0f, -10.0f, -5.0f); // bottom left it is also ks/kd[1] so green
	Plane_lightPosition[2] = Vec3(10.0f, -10.0f, -5.0f); // bottom right it is also ks/kd[2] so blue
	Plane_lightPosition[3] = Vec3(10.0f, 10.0f, -5.0f); // top right it is also ks/kd[3] so orangeish yellow
	Plane_lightPosition[4] = Vec3(0.0f, -10.0f, -5.0f); // bottom it is also ks/kd[4] so a bit purple

	Plane_ks[0] = Vec4(0.4, 0.1, 0.1, 0.0);  // Soft red highlight
	Plane_ks[1] = Vec4(0.1, 0.4, 0.1, 0.0);  // Soft green highlight
	Plane_ks[2] = Vec4(0.1, 0.1, 0.4, 0.0);  // Soft blue highlight
	Plane_ks[3] = Vec4(0.4, 0.4, 0.1, 0.0);  // Warm yellow
	Plane_ks[4] = Vec4(0.8, 0.1, 0.8, 0.0);  // Magenta touch

	Plane_kd[0] = Vec4(0.7, 0.3, 0.3, 0.0);  // Soft red-pink
	Plane_kd[1] = Vec4(0.3, 0.7, 0.3, 0.0);  // Soft green
	Plane_kd[2] = Vec4(0.3, 0.3, 0.7, 0.0);  // Soft blue
	Plane_kd[3] = Vec4(0.8, 0.5, 0.2, 0.0);  // Subdued orange
	Plane_kd[4] = Vec4(0.8, 0.3, 0.8, 0.0);  // Gentle purple

	return true;
}

void Scene0p::OnDestroy() {
	Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);
	sphere->OnDestroy();
	delete sphere;
	sphere = nullptr;

	mesh->OnDestroy();
	delete mesh;
	mesh = nullptr;

	shader->OnDestroy();
	delete shader;
	shader = nullptr;

	texture->OnDestroy();
	delete texture;
	texture = nullptr;

	plane->OnDestroy();
	delete plane;
	plane = nullptr;

	planeMesh->OnDestroy();
	delete planeMesh;
	planeMesh = nullptr;

	planeShader->OnDestroy();
	delete planeShader;
	planeShader = nullptr;

	planeText->OnDestroy();
	delete planeText;
	planeText = nullptr;

	plane_2->OnDestroy();
	delete plane_2;
	plane_2 = nullptr;

	planeMesh_2->OnDestroy();
	delete planeMesh_2;
	planeMesh_2 = nullptr;

	planeShader_2->OnDestroy();
	delete planeShader_2;
	planeShader_2 = nullptr;

	planeText_2->OnDestroy();
	delete planeText_2;
	planeText_2 = nullptr;

	plane_3->OnDestroy();
	delete plane_3;
	plane_3 = nullptr;

	planeMesh_3->OnDestroy();
	delete planeMesh_3;
	planeMesh_3 = nullptr;

	planeShader_3->OnDestroy();
	delete planeShader_3;
	planeShader_3 = nullptr;

	planeText_3->OnDestroy();
	delete planeText_3;
	planeText_3 = nullptr;

	camera->OnDestroy();
	delete camera;
	camera = nullptr;

	if (planeShape) {
		delete planeShape;
		planeShape = nullptr;
	}
	if (planeShape_2) {
		delete planeShape_2;
		planeShape_2 = nullptr;
	}
	if (planeShape_3) {
		delete planeShape_3;
		planeShape_3 = nullptr;
	}
}

void Scene0p::HandleEvents(const SDL_Event &sdlEvent) {
	trackball.HandleEvents(sdlEvent);
	switch( sdlEvent.type ) {
	case SDL_KEYDOWN:
		if (SDL_GetModState() & KMOD_LSHIFT) {
			switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_W:
				rotationForPlane2 = QMath::angleAxisRotation(-1.0f, Vec3(1.0f, 0.0f, 0.0f));
				plane_2->orientation *= rotationForPlane2;
				planeShape_2->normal = QMath::rotate(planeShape_2->normal, rotationForPlane2);
				planeAngle_2++;
				break;


			case SDL_SCANCODE_S:
				rotationForPlane2 = QMath::angleAxisRotation(1.0f, Vec3(1.0f, 0.0f, 0.0f));
				plane_2->orientation *= rotationForPlane2;
				planeShape_2->normal = QMath::rotate(planeShape_2->normal, rotationForPlane2);
				planeAngle_2--;
				break;

			case SDL_SCANCODE_A:
				rotationForPlane2 = QMath::angleAxisRotation(1.0f, Vec3(0.0f, 0.0f, 1.0f));
				plane_2->orientation *= rotationForPlane2;
				planeShape_2->normal = QMath::rotate(planeShape_2->normal, rotationForPlane2);
				planeAngle_2X++;
				break;

			case SDL_SCANCODE_D:
				rotationForPlane2 = QMath::angleAxisRotation(-1.0f, Vec3(0.0f, 0.0f, 1.0f));
				plane_2->orientation *= rotationForPlane2;
				planeShape_2->normal = QMath::rotate(planeShape_2->normal, rotationForPlane2);
				planeAngle_2X--;
				break;

			case SDL_SCANCODE_LEFT:
				rotationForPlane2 = QMath::angleAxisRotation(-1.0f, Vec3(0.0f, 1.0f, 0.0f));
				plane_2->orientation *= rotationForPlane2;
				planeShape_2->normal = QMath::rotate(planeShape_2->normal, rotationForPlane2);
				break;

			case SDL_SCANCODE_RIGHT:
				rotationForPlane2 = QMath::angleAxisRotation(1.0f, Vec3(0.0f, 1.0f, 0.0f));
				plane_2->orientation *= rotationForPlane2;
				planeShape_2->normal = QMath::rotate(planeShape_2->normal, rotationForPlane2);
				break;
			}
		}
		else if (SDL_GetModState() & KMOD_RSHIFT) {
			switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_W:
				rotationForPlane3 = QMath::angleAxisRotation(-1.0f, Vec3(1.0f, 0.0f, 0.0f));
				plane_3->orientation *= rotationForPlane3;
				planeShape_3->normal = QMath::rotate(planeShape_3->normal, rotationForPlane3);
				planeAngle_3++;
				break;


			case SDL_SCANCODE_S:
				rotationForPlane3 = QMath::angleAxisRotation(1.0f, Vec3(1.0f, 0.0f, 0.0f));
				plane_3->orientation *= rotationForPlane3;
				planeShape_3->normal = QMath::rotate(planeShape_3->normal, rotationForPlane3);
				planeAngle_3--;
				break;

			case SDL_SCANCODE_A:
				rotationForPlane3 = QMath::angleAxisRotation(1.0f, Vec3(0.0f, 0.0f, 1.0f));
				plane_3->orientation *= rotationForPlane3;
				planeShape_3->normal = QMath::rotate(planeShape_3->normal, rotationForPlane3);
				planeAngle_3X++;
				break;

			case SDL_SCANCODE_D:
				rotationForPlane3 = QMath::angleAxisRotation(-1.0f, Vec3(0.0f, 0.0f, 1.0f));
				plane_3->orientation *= rotationForPlane3;
				planeShape_3->normal = QMath::rotate(planeShape_3->normal, rotationForPlane3);
				planeAngle_3X--;
				break;

			case SDL_SCANCODE_LEFT:
				rotationForPlane3 = QMath::angleAxisRotation(-1.0f, Vec3(0.0f, 1.0f, 0.0f));
				plane_3->orientation *= rotationForPlane3;
				planeShape_3->normal = QMath::rotate(planeShape_3->normal, rotationForPlane3);
				break;

			case SDL_SCANCODE_RIGHT:
				rotationForPlane3 = QMath::angleAxisRotation(1.0f, Vec3(0.0f, 1.0f, 0.0f));
				plane_3->orientation *= rotationForPlane3;
				planeShape_3->normal = QMath::rotate(planeShape_3->normal, rotationForPlane3);
				break;
			}
		}
		else {
			switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_L:
				drawInWireMode = !drawInWireMode;
				break;

			case SDL_SCANCODE_W:
				rotation = QMath::angleAxisRotation(-1.0f, Vec3(1.0f, 0.0f, 0.0f));
				plane->orientation *= rotation;
				planeShape->normal = QMath::rotate(planeShape->normal, rotation);
				planeAngle++;
				//std::cout << planeShape->normal.x << ", y: " << planeShape->normal.y << ", z: " << planeShape->normal.z << std::endl;
				break;

			case SDL_SCANCODE_S:
				rotation = QMath::angleAxisRotation(1.0f, Vec3(1.0f, 0.0f, 0.0f));
				plane->orientation *= rotation;
				planeShape->normal = QMath::rotate(planeShape->normal, rotation);
				planeAngle--;
				break;

			case SDL_SCANCODE_A:
				rotation = QMath::angleAxisRotation(1.0f, Vec3(0.0f, 0.0f, 1.0f));
				plane->orientation *= rotation;
				planeShape->normal = QMath::rotate(planeShape->normal, rotation);
				planeAngleX++;
				break;

			case SDL_SCANCODE_D:
				rotation = QMath::angleAxisRotation(-1.0f, Vec3(0.0f, 0.0f, 1.0f));
				plane->orientation *= rotation;
				planeShape->normal = QMath::rotate(planeShape->normal, rotation);
				planeAngleX--;
				break;

			case SDL_SCANCODE_LEFT:
				rotation = QMath::angleAxisRotation(-1.0f, Vec3(0.0f, 1.0f, 0.0f));
				plane->orientation *= rotation;
				planeShape->normal = QMath::rotate(planeShape->normal, rotation);
				break;

			case SDL_SCANCODE_RIGHT:
				rotation = QMath::angleAxisRotation(1.0f, Vec3(0.0f, 1.0f, 0.0f));
				plane->orientation *= rotation;
				planeShape->normal = QMath::rotate(planeShape->normal, rotation);
				break;
			}
		}
		break;


		break;

	case SDL_MOUSEMOTION:          
		break;

	case SDL_MOUSEBUTTONDOWN:  
		if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
			move = true;
		}
		break; 

	case SDL_MOUSEBUTTONUP:  
		if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
			move = false;
		}
	break;

	default:
		break;
    }
}

void Scene0p::resetAll(){

	plane->orientation = plane1_rotationHolder;
	plane_2->orientation = plane2_rotationHolder;
	plane_3->orientation = plane3_rotationHolder;

	planeShape->normal = Vec3(0.0f, 0.0f, -1.0f);
	planeShape_2->normal = Vec3(0.0f, 0.0f, -1.0f);
	planeShape_3->normal = Vec3(0.0f, 0.0f, -1.0f);
	planeShape->normal = QMath::rotate(planeShape->normal, plane1_rotationHolder);
	planeShape_2->normal = QMath::rotate(planeShape_2->normal, plane2_rotationHolder);
	planeShape_3->normal = QMath::rotate(planeShape_3->normal, plane3_rotationHolder);

	planeAngle = 0.0f;
	planeAngleX = 0.0f;
	planeAngle_2 = 0.0f;
	planeAngle_2X = 0.0f;
	planeAngle_3 = 0.0f;
	planeAngle_3X = 0.0f;

	sphere->pos = Vec3(0.0f, 2.0f, 0.0f);
	sphere->vel = Vec3(0.0f, 0.0f, 0.0f);
	sphere->accel = Vec3(0.0f, 0.0f, 0.0f);
	sphere->angVel = Vec3(0.0f, 0.0f, 0.0f);
	sphere->angAcc = Vec3(0.0f, 0.0f, 0.0f);
	ballOnPlane1 = true;
}

void Scene0p::Update(const float deltaTime) {
//	std::cout << planeAngle_2 - planeAngle << std::endl;
//	std::cout << planeAngle_3X - planeAngleX << std::endl;
//	std::cout << ballOnPlane2 << std::endl;
//	std::cout << ballOnPlane3 << std::endl;
	//std::cout << "spherePos.x: " << sphere->pos.x << ", y: " << sphere->pos.y << ", z" << sphere->pos.z << std::endl;
	sphere->Update(deltaTime);
	float const gravity = -9.8f;
	float c = 0.2f;
	Vec3 gravityAcc(0.0f, gravity * sphere->mass, 0.0f);
	Vec3 force = (sphere->vel * -c) + gravityAcc;
	// Apply forces to the sphere
	sphere->ApplyForce(force);


	cameraOrientation = trackball.getQuat();
	Vec3 offset = Vec3(0.0f, 1.0f, 30.0f);
	Vec3 rotatedOffset = QMath::rotate(offset, cameraOrientation);
	cameraPos = sphere->pos + rotatedOffset;
	camera->SetView(cameraOrientation, cameraPos);

//	camera->SetView(QMath::inverse(Quaternion(1,sphere->pos)), cameraPos);
	//sphere->pos.y > -1.0f || ballOnPlane2 == true
	if (VMath::mag(sphere->pos - plane_2->pos) < VMath::mag(sphere->pos - plane->pos) && planeAngle_2 - planeAngle >= -3 || ballOnPlane2 == true) {
		ballOnPlane2 = true;
		if (ballOnPlane2 != false) {
			Vec3 up2 = Vec3(0.0f, 1.0f, 0.0f);
		//	std::cout << "planeshape2 normal X: " << planeShape_2->normal.x << ", Y: " << planeShape_2->normal.y << ", Z: " << planeShape_2->normal.z << std::endl;
			float cos_angle2 = VMath::dot(up2, planeShape_2->normal);
			float angle2 = acosf(cos_angle2);
			Vec3 torqueDirection2 = VMath::cross(up2, planeShape_2->normal);
			float torqueMag2 = sphere->mass * sphere->radius * sin(angle2);
			Vec3 totalTorque2 = torqueMag2 * torqueDirection2;
			sphere->ApplyTorque(totalTorque2);

			// Update angular velocity based on the new angular acceleration
			sphere->UpdateAngularVel(deltaTime);
			// Update orientation based on angular velocity
			sphere->UpdateOrientation(deltaTime);
			//float d2 = VMath::dot(planeShape_2->normal, sphere->pos);

			sphere->UpdateVel(deltaTime);
			sphere->updatePos(deltaTime);
			float d2 = VMath::dot(planeShape_2->normal, sphere->pos - plane_2->pos);
			if (d2 < 1.0f && sphere->pos.z > -31.0f && sphere->pos.x < 10.0f) {
				sphere->vel = VMath::cross(sphere->angVel, planeShape_2->normal);
				sphere->pos += (d2 - 1) * -VMath::normalize(planeShape_2->normal);
			}
			if (VMath::mag(sphere->pos - plane->pos) < VMath::mag(sphere->pos - plane_2->pos)) {
				ballOnPlane3 = false;
				ballOnPlane2 = false;
			}


			sphere->UpdateVel(deltaTime);
			sphere->updatePos(deltaTime);
		}
	}
	else if (VMath::mag(sphere->pos - plane_3->pos) < VMath::mag(sphere->pos - plane->pos) && planeAngle_3X - planeAngleX >= -3 || ballOnPlane3 == true) {
		ballOnPlane3 = true;
		Vec3 up3 = Vec3(0.0f, 1.0f, 0.0f);
	//	std::cout << "planeshape2 normal X: " << planeShape_3->normal.x << ", Y: " << planeShape_3->normal.y << ", Z: " << planeShape_3->normal.z << std::endl;
		float cos_angle3 = VMath::dot(up3, planeShape_3->normal);
		float angle3 = acosf(cos_angle3);
		
		Vec3 torqueDirection3 = VMath::cross(up3, planeShape_3->normal);
		float torqueMag3 = sphere->mass * sphere->radius * sin(angle3);
		Vec3 totalTorque3 = torqueMag3 * torqueDirection3;
		sphere->ApplyTorque(totalTorque3);

		// Update angular velocity based on the new angular acceleration
		sphere->UpdateAngularVel(deltaTime);
		// Update orientation based on angular velocity
		sphere->UpdateOrientation(deltaTime);
		//float d2 = VMath::dot(planeShape_2->normal, sphere->pos);
		float d3 = VMath::dot(planeShape_3->normal, sphere->pos - plane_3->pos);
		
		if (sphere->pos.x > -31.0f && sphere->pos.z > -10.0f && sphere->pos.z < 10.0f) {
			if (d3 < 1.0f) {
				sphere->vel = VMath::cross(sphere->angVel, planeShape_3->normal);
				sphere->pos += (d3 - 1) * -VMath::normalize(planeShape_3->normal);
			}
		}
		if (VMath::mag(sphere->pos - plane->pos) < VMath::mag(sphere->pos - plane_3->pos)) {
			ballOnPlane3 = false;
			ballOnPlane2 = false;
		}
		sphere->UpdateVel(deltaTime);
		sphere->updatePos(deltaTime);
		
	}
	else {
		if (ballOnPlane1 == false) {
			if (VMath::mag(sphere->pos - plane_3->pos) > VMath::mag(sphere->pos - plane_2->pos) && planeAngle_3X - planeAngleX <= -2 && sphere->pos.y > -8.0f
				|| VMath::mag(sphere->pos - plane_3->pos) > VMath::mag(sphere->pos - plane_2->pos) && sphere->pos.y > 1.5f) {
				ballOnPlane1 = true;
			}
			else if (VMath::mag(sphere->pos - plane_3->pos) < VMath::mag(sphere->pos - plane_2->pos) && planeAngle_2 - planeAngle <= -2 && sphere->pos.y > -8.0f
				|| VMath::mag(sphere->pos - plane_3->pos) < VMath::mag(sphere->pos - plane_2->pos) && sphere->pos.y > 1.5f) {
				ballOnPlane1 = true;
			}
			else {
				ballOnPlane1 = false;
			}
		}
		ballOnPlane3 = false;
		ballOnPlane2 = false;
	}
	if (ballOnPlane1 == true){
		ballOnPlane3 = false;
		ballOnPlane2 = false;
		ballOnPlane1 = true;
		Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
		//std::cout << "planeshape normal X: " << planeShape->normal.x << ", Y: " << planeShape->normal.y << ", Z: " << planeShape->normal.z << std::endl;
		float cos_angle = VMath::dot(up, planeShape->normal);
		float angle = acosf(cos_angle);
		Vec3 torqueDirection = VMath::cross(up, planeShape->normal);
		float torqueMag = sphere->mass * sphere->radius * sin(angle);
		Vec3 totalTorque = torqueMag * torqueDirection;
		sphere->ApplyTorque(totalTorque);
		
		// Update angular velocity based on the new angular acceleration
		sphere->UpdateAngularVel(deltaTime);
		// Update orientation based on angular velocity
		sphere->UpdateOrientation(deltaTime);
		float d = VMath::dot(planeShape->normal, sphere->pos);
		if (d < 1.0f && sphere->pos.z > -10.0f && sphere->pos.x < 10.0f && sphere->pos.z < 10.0f && sphere->pos.x > -10.0f) {
			sphere->vel = VMath::cross(sphere->angVel, planeShape->normal);
			sphere->pos += (d - 1) * -VMath::normalize(planeShape->normal);
		}
		else if(sphere->pos.z < -10.0f || sphere->pos.x > 10.0f || sphere->pos.z > 10.0f || sphere->pos.x < -10.0f){
			ballOnPlane1 = false;
			
		}
		sphere->UpdateVel(deltaTime);
		sphere->updatePos(deltaTime);
	}

//	sphere->vel = VMath::cross(sphere->angVel, planeShape->normal);
	sphere->UpdateVel(deltaTime);
	sphere->updatePos(deltaTime);

	// Update angular velocity based on the new angular acceleration
	sphere->UpdateAngularVel(deltaTime);
	// Update orientation based on angular velocity
	sphere->UpdateOrientation(deltaTime);
	if (sphere->pos.y <= -150.0f) {
		SDL_Event event;
		event.type = SDL_KEYDOWN;
		event.key.keysym.scancode = SDL_SCANCODE_F3;
		event.key.keysym.sym = SDLK_F3;
		event.key.state = SDL_PRESSED;
		SDL_PushEvent(&event);  // Inject fake key event
		resetAll();
	}
}

void Scene0p::Render() const {
	///// Set the background color then clear the screen
	glEnable(GL_DEPTH_TEST);
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	camera->RenderSkyBox();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(shader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphere->GetModelMatrix());
	
	glUniform3fv(shader->GetUniformID("lightPosition[0]"), 5, *lightPosition);
	glUniform4fv(shader->GetUniformID("ks[0]"), 5, *ks);
	glUniform4fv(shader->GetUniformID("kd[0]"), 5, *kd);
	mesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//glCullFace(GL_FRONT);
	glDisable(GL_CULL_FACE);
	glUseProgram(planeShader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, planeText->getTextureID());
	glUniformMatrix4fv(planeShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(planeShader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(planeShader->GetUniformID("planeModelMatrix"), 1, GL_FALSE, plane->GetModelMatrix());


	glUniform3fv(planeShader->GetUniformID("lightPosition[0]"), 5, *Plane_lightPosition);
	glUniform4fv(planeShader->GetUniformID("ks[0]"), 5, *Plane_ks);
	glUniform4fv(planeShader->GetUniformID("kd[0]"), 5, *Plane_kd);

	planeMesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(planeShader_2->GetProgram());
	glBindTexture(GL_TEXTURE_2D, planeText_2->getTextureID());
	glUniformMatrix4fv(planeShader_2->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(planeShader_2->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(planeShader_2->GetUniformID("planeModelMatrix"), 1, GL_FALSE, plane_2->GetModelMatrix());
	glUniform3fv(planeShader_2->GetUniformID("lightPosition[0]"), 5, *Plane_lightPosition);
	glUniform4fv(planeShader_2->GetUniformID("ks[0]"), 5, *Plane_ks);
	glUniform4fv(planeShader_2->GetUniformID("kd[0]"), 5, *Plane_kd);

	planeMesh_2->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(planeShader_3->GetProgram());
	glBindTexture(GL_TEXTURE_2D, planeText_3->getTextureID());
	glUniformMatrix4fv(planeShader_3->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(planeShader_3->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(planeShader_3->GetUniformID("planeModelMatrix"), 1, GL_FALSE, plane_3->GetModelMatrix());
	glUniform3fv(planeShader_3->GetUniformID("lightPosition[0]"), 5, *Plane_lightPosition);
	glUniform4fv(planeShader_3->GetUniformID("ks[0]"), 5, *Plane_ks);
	glUniform4fv(planeShader_3->GetUniformID("kd[0]"), 5, *Plane_kd);
	planeMesh_3->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glUseProgram(0);

}



	
