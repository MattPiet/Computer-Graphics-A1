#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene1p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"





Scene1p::Scene1p() :sphere{ nullptr }, shader{ nullptr }, mesh{ nullptr },
drawInWireMode{ true } {
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene1p::~Scene1p() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}



bool Scene1p::OnCreate() {
	Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);
	sphere = new Body();
	sphere->OnCreate();
	sphere->pos = Vec3(15.0f, 1.0f, 0.0f);
	
	sphere2 = new Body();
	sphere2->OnCreate();
	sphere2->pos = Vec3(-15.0f, 1.0f, 1.0f);
	for (int i = 0; i < 2; i++) {
		arrayOfSpheres[i] = new Body();
		arrayOfSpheres[i]->OnCreate();
	}

	arrayOfSpheres[0]->pos = Vec3(-15.0f, 1.0f, -1.0f);
	arrayOfSpheres[1]->pos = Vec3(-13.0f, 1.0f, 0.0f);
	//arrayOfSpheres[2]->pos = sphere2->pos = Vec3(-14.0f, 1.0f, 0.0f);


	plane = new Body();
	plane->OnCreate();


	planeShape = new Plane();
	planeShape->normal = Vec3(0.0f, 0.0f, -1.0f);

	
	mesh = new Mesh("meshes/Sphere.obj");
	mesh->OnCreate();


	sphere2Mesh = new Mesh("meshes/Sphere.obj");
	sphere2Mesh->OnCreate();

	sphere3Mesh = new Mesh("meshes/Sphere.obj");
	sphere3Mesh->OnCreate();

	sphere4Mesh = new Mesh("meshes/Sphere.obj");
	sphere4Mesh->OnCreate();

	planeMesh = new Mesh("meshes/Plane.obj");
	planeMesh->OnCreate();


	texture = new Texture();
	texture->LoadImage("textures/blank.png");
	eightBall = new Texture();

	eightBall->LoadImage("textures/8ball.png");


	planeText = new Texture();
	planeText->LoadImage("textures/pool_table.png");

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


	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	// 0.0f, 0.0f, 0.0f
	rotation = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
	plane->orientation *= rotation;
	sphere2->orientation *= rotation;
	sphere2->orientation *= QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f)); 
	for (int i = 0; i < 2; i++) {
		arrayOfSpheres[i]->orientation *= rotation;
		arrayOfSpheres[i]->orientation *= QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f));
	}
	planeShape->normal = QMath::rotate(planeShape->normal, rotation);
	plane->size *= 2;
	plane->size.x *= 2;
	plane1_rotationHolder = rotation;
	std::cout << planeShape->normal.x << ", y: " << planeShape->normal.y << ", z: " << planeShape->normal.z << std::endl;


	plane->pos = Vec3(0.0f, 0.0f, 0.0f);

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
	trackball.Trackingx = false;
	trackball.Trackingz = false;
	std::vector<std::string> skyTexts = {
		"textures/px.png",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X 
		"textures/nx.png",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
		"textures/py.png",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
		"textures/ny.png",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
		"textures/pz.png",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z// this is technically the back side but because insides are
		"textures/nz.png" 
	};					// flipped when looking inside a skybox  i need to make it the front side the same applies with the back jpg
	camera->setSkyBox(skyTexts);
	return true;
}

void Scene1p::OnDestroy() {
	Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);
	sphere->OnDestroy();
	delete sphere;

	mesh->OnDestroy();
	delete mesh;

	sphere2->OnDestroy();
	delete sphere2;

	sphere2Mesh->OnDestroy();
	delete sphere2Mesh;

	shader->OnDestroy();
	delete shader;

	plane->OnDestroy();
	delete plane;

	planeMesh->OnDestroy();
	delete planeMesh;

	planeShader->OnDestroy();
	delete planeShader;

	texture->OnDestroy();
	delete texture;

	planeText->OnDestroy();
	delete planeText;

	camera->OnDestroy();
	delete camera;

}

void Scene1p::HandleEvents(const SDL_Event& sdlEvent) {
	trackball.HandleEvents(sdlEvent);
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		
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

			case SDL_SCANCODE_SPACE:
				impulseDirWorldSpace = QMath::rotate(impulseDirCameraSpace,
					trackball.getQuat());
				if (power < 8.0f) {
					power += 0.2;
				}
				std::cout << "Power: " << power << std::endl;
		
				break;
			case SDL_SCANCODE_R:
				resetAll();
				break;
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
	case SDL_KEYUP:
	case SDL_SCANCODE_SPACE:
		sphere->vel = impulseDirWorldSpace * power;
		sphere->angVel = VMath::cross(Vec3(0.0f, 1.0f, 0.0f), sphere->vel);
		power = 0.0f;
		break;
		break;

	default:
		break;
	}
}

void Scene1p::resetAll() {

	plane->orientation = plane1_rotationHolder;

	planeShape->normal = Vec3(0.0f, 0.0f, -1.0f);
	planeShape->normal = QMath::rotate(planeShape->normal, plane1_rotationHolder);

	planeAngle = 0.0f;
	planeAngleX = 0.0f;
	

	
	
	sphere->pos = Vec3(15.0f, 1.0f, 0.0f);
	sphere->vel = Vec3(0.0f, 0.0f, 0.0f);
	sphere->accel = Vec3(0.0f, 0.0f, 0.0f);
	sphere->angVel = Vec3(0.0f, 0.0f, 0.0f);
	sphere->angAcc = Vec3(0.0f, 0.0f, 0.0f);
	arrayOfSpheres[0]->pos = Vec3(-15.0f, 1.0f, -1.0f);
	arrayOfSpheres[1]->pos = Vec3(-13.0f, 1.0f, 0.0f);

	for (int i = 0; i < 2; i++) {
		arrayOfSpheres[i]->vel = Vec3(0.0f, 0.0f, 0.0f);
		arrayOfSpheres[i]->accel = Vec3(0.0f, 0.0f, 0.0f);
		arrayOfSpheres[i]->angVel = Vec3(0.0f, 0.0f, 0.0f);
		arrayOfSpheres[i]->angAcc = Vec3(0.0f, 0.0f, 0.0f);
	}
	sphere2->pos = Vec3(-15.0f, 1.0f, 1.0f);
	sphere2->vel = Vec3(0.0f, 0.0f, 0.0f);
	sphere2->accel = Vec3(0.0f, 0.0f, 0.0f);
	sphere2->angVel = Vec3(0.0f, 0.0f, 0.0f);
	sphere2->angAcc = Vec3(0.0f, 0.0f, 0.0f);
	ballOnPlane1 = true;
	Sphere2OnPlane1 = true;
	Sphere3OnPlane1 = true;
	Sphere4OnPlane1 = true;
}

void Scene1p::Update(const float deltaTime) {
	sphere->Update(deltaTime);

	cameraOrientation = trackball.getQuat(); 
	Vec3 offset = Vec3(0.0f, 1.0f, 30.0f); 
	Vec3 rotatedOffset = QMath::rotate(offset, cameraOrientation);
	cameraPos = sphere->pos + rotatedOffset;  
	camera->SetView(cameraOrientation, cameraPos);
	//sphere->pos.print();
	sphere->UpdateVel(deltaTime);
	sphere->updatePos(deltaTime);
	if (ballOnPlane1 == true) {
		ballOnPlane1 = true;
		sphere->ApplyForce(sphere->ApplyFriction());
		Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
		sphere->UpdateOrientation(deltaTime);
		float d = VMath::dot(planeShape->normal, sphere->pos);
		if (d < 1.0f && sphere->pos.z > -10.0f && sphere->pos.x < 20.0f && sphere->pos.z < 10.0f && sphere->pos.x > -20.0f) {
			sphere->vel = VMath::cross(sphere->angVel, planeShape->normal);
			sphere->pos += (d - 1) * -VMath::normalize(planeShape->normal);
		}
		else if (sphere->pos.z < -10.0f || sphere->pos.x > 20.0f || sphere->pos.z > 10.0f || sphere->pos.x < -20.0f) {
			float const gravity = -9.8f;
			float c = 0.2f;
			Vec3 gravityAcc(0.0f, gravity * sphere->mass, 0.0f);
			Vec3 forceOfGravity = (sphere->vel * -c) + gravityAcc;
			sphere->ApplyForce(forceOfGravity);
			ballOnPlane1 = false;
		}
		sphere->UpdateVel(deltaTime);
		sphere->updatePos(deltaTime);
	}

	sphere->UpdateVel(deltaTime);
	sphere->updatePos(deltaTime);

	sphere->UpdateAngularVel(deltaTime);

	sphere->UpdateOrientation(deltaTime);

	
		arrayOfSpheres[0]->Update(deltaTime);
		if (Sphere3OnPlane1 == true) {
			arrayOfSpheres[0]->Update(deltaTime);
			// Apply forces to the sphere
			arrayOfSpheres[0]->ApplyForce(sphere2->ApplyFriction());
			Sphere3OnPlane1 = true;
			Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
			arrayOfSpheres[0]->UpdateAngularVel(deltaTime);
			// Update orientation based on angular velocity
			arrayOfSpheres[0]->UpdateOrientation(deltaTime);
			float d = VMath::dot(planeShape->normal, arrayOfSpheres[0]->pos);
			if (d < 1.0f && arrayOfSpheres[0]->pos.z > -10.0f && arrayOfSpheres[0]->pos.x < 20.0f && arrayOfSpheres[0]->pos.z < 10.0f && arrayOfSpheres[0]->pos.x > -20.0f) {
				arrayOfSpheres[0]->vel = VMath::cross(arrayOfSpheres[0]->angVel, planeShape->normal);
				arrayOfSpheres[0]->pos += (d - 1) * -VMath::normalize(planeShape->normal);
			}
			else if (arrayOfSpheres[0]->pos.z < -10.0f || arrayOfSpheres[0]->pos.x > 20.0f || arrayOfSpheres[0]->pos.z > 10.0f || arrayOfSpheres[0]->pos.x < -20.0f) {
				float const gravity = -9.8f;
				float c = 0.2f;
				Vec3 gravityAcc(0.0f, gravity * arrayOfSpheres[0]->mass, 0.0f);
				Vec3 forceOfGravity = (arrayOfSpheres[1]->vel * -c) + gravityAcc;
				arrayOfSpheres[0]->ApplyForce(forceOfGravity);
				Sphere3OnPlane1 = false;
			}
			arrayOfSpheres[0]->UpdateVel(deltaTime);
			arrayOfSpheres[0]->updatePos(deltaTime);
		}

		//	sphere->vel = VMath::cross(sphere->angVel, planeShape->normal);
		arrayOfSpheres[0]->UpdateVel(deltaTime);
		arrayOfSpheres[0]->updatePos(deltaTime);

		// Update angular velocity based on the new angular acceleration
		arrayOfSpheres[0]->UpdateAngularVel(deltaTime);
		// Update orientation based on angular velocity
		arrayOfSpheres[0]->UpdateOrientation(deltaTime);
		if (sphere->pos.y <= -150.0f) {
			//SDL_Event event;
			//event.type = SDL_KEYDOWN;
			//event.key.keysym.scancode = SDL_SCANCODE_F1;
			//event.key.keysym.sym = SDLK_F1;
			//event.key.state = SDL_PRESSED;
			//SDL_PushEvent(&event);  
			resetAll();
		}
		else if (sphere2->pos.y <= -150.0f) {
			resetAll();
		}
		else if (arrayOfSpheres[0]->pos.y <= -150.0f) {
			resetAll();
		}
		Collision::SphereSphereCollisionResponse(sphere2, arrayOfSpheres[0]);
		Collision::SphereSphereCollisionResponse(sphere, arrayOfSpheres[0]);
		Collision::SphereSphereCollisionResponse(arrayOfSpheres[0], arrayOfSpheres[1]);

		arrayOfSpheres[1]->Update(deltaTime);
		if (Sphere4OnPlane1 == true) {
			arrayOfSpheres[1]->Update(deltaTime);
			// Apply forces to the sphere
			arrayOfSpheres[1]->ApplyForce(sphere2->ApplyFriction());
			Sphere4OnPlane1 = true;
			Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
			arrayOfSpheres[1]->UpdateAngularVel(deltaTime);
			// Update orientation based on angular velocity
			arrayOfSpheres[1]->UpdateOrientation(deltaTime);
			float d = VMath::dot(planeShape->normal, arrayOfSpheres[1]->pos);
			if (d < 1.0f && arrayOfSpheres[1]->pos.z > -10.0f && arrayOfSpheres[1]->pos.x < 20.0f && arrayOfSpheres[1]->pos.z < 10.0f && arrayOfSpheres[1]->pos.x > -20.0f) {
				arrayOfSpheres[1]->vel = VMath::cross(arrayOfSpheres[1]->angVel, planeShape->normal);
				arrayOfSpheres[1]->pos += (d - 1) * -VMath::normalize(planeShape->normal);
			}
			else if (arrayOfSpheres[1]->pos.z < -10.0f || arrayOfSpheres[1]->pos.x > 20.0f || arrayOfSpheres[1]->pos.z > 10.0f || arrayOfSpheres[1]->pos.x < -20.0f) {
				float const gravity = -9.8f;
				float c = 0.2f;
				Vec3 gravityAcc(0.0f, gravity * arrayOfSpheres[1]->mass, 0.0f);
				Vec3 forceOfGravity = (arrayOfSpheres[1]->vel * -c) + gravityAcc;
				arrayOfSpheres[1]->ApplyForce(forceOfGravity);
				Sphere4OnPlane1 = false;
			}
			arrayOfSpheres[1]->UpdateVel(deltaTime);
			arrayOfSpheres[1]->updatePos(deltaTime);
		}

		//	sphere->vel = VMath::cross(sphere->angVel, planeShape->normal);
		arrayOfSpheres[1]->UpdateVel(deltaTime);
		arrayOfSpheres[1]->updatePos(deltaTime);

		// Update angular velocity based on the new angular acceleration
		arrayOfSpheres[1]->UpdateAngularVel(deltaTime);
		// Update orientation based on angular velocity
		arrayOfSpheres[1]->UpdateOrientation(deltaTime);
		if (sphere->pos.y <= -150.0f) {
			//SDL_Event event;
			//event.type = SDL_KEYDOWN;
			//event.key.keysym.scancode = SDL_SCANCODE_F1;
			//event.key.keysym.sym = SDLK_F1;
			//event.key.state = SDL_PRESSED;
			//SDL_PushEvent(&event);  
			resetAll();
		}
		else if (sphere2->pos.y <= -150.0f) {
			resetAll();
		}
		else if (arrayOfSpheres[1]->pos.y <= -150.0f) {
			resetAll();
		}
		Collision::SphereSphereCollisionResponse(sphere2, arrayOfSpheres[1]);
		Collision::SphereSphereCollisionResponse(sphere, arrayOfSpheres[1]);
		Collision::SphereSphereCollisionResponse(arrayOfSpheres[0], arrayOfSpheres[1]);


	
	sphere2->Update(deltaTime);
	if (Sphere2OnPlane1 == true) {
		sphere2->Update(deltaTime);
		// Apply forces to the sphere
		sphere2->ApplyForce(sphere2->ApplyFriction());
		Sphere2OnPlane1 = true;
		Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
		sphere2->UpdateAngularVel(deltaTime);
		// Update orientation based on angular velocity
		sphere2->UpdateOrientation(deltaTime);
		float d = VMath::dot(planeShape->normal, sphere2->pos);
		if (d < 1.0f && sphere2->pos.z > -10.0f && sphere2->pos.x < 20.0f && sphere2->pos.z < 10.0f && sphere2->pos.x > -20.0f) {
			sphere2->vel = VMath::cross(sphere2->angVel, planeShape->normal);
			sphere2->pos += (d - 1) * -VMath::normalize(planeShape->normal);
		}
		else if (sphere2->pos.z < -10.0f || sphere2->pos.x > 20.0f || sphere2->pos.z > 10.0f || sphere2->pos.x < -20.0f) {
			float const gravity = -9.8f;
			float c = 0.2f;
			Vec3 gravityAcc(0.0f, gravity * sphere2->mass, 0.0f);
			Vec3 forceOfGravity = (sphere2->vel * -c) + gravityAcc;
			sphere2->ApplyForce(forceOfGravity);
			Sphere2OnPlane1 = false;
		}
		sphere2->UpdateVel(deltaTime);
		sphere2->updatePos(deltaTime);
	}

	//	sphere->vel = VMath::cross(sphere->angVel, planeShape->normal);
	sphere2->UpdateVel(deltaTime);
	sphere2->updatePos(deltaTime);

	// Update angular velocity based on the new angular acceleration
	sphere2->UpdateAngularVel(deltaTime);
	// Update orientation based on angular velocity
	sphere2->UpdateOrientation(deltaTime);
	if (sphere->pos.y <= -150.0f) {
		//SDL_Event event;
		//event.type = SDL_KEYDOWN;
		//event.key.keysym.scancode = SDL_SCANCODE_F1;
		//event.key.keysym.sym = SDLK_F1;
		//event.key.state = SDL_PRESSED;
		//SDL_PushEvent(&event);  
		resetAll();
	}
	else if (sphere2->pos.y <= -150.0f) {
		resetAll();
	}
	Collision::SphereSphereCollisionResponse(sphere, sphere2);

}

void Scene1p::Render() const {
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
	mesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(shader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, eightBall->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphere2->GetModelMatrix());
	sphere2Mesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);

	
		glUseProgram(shader->GetProgram());
		glBindTexture(GL_TEXTURE_2D, eightBall->getTextureID());
		glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
		glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, arrayOfSpheres[1]->GetModelMatrix());
		sphere3Mesh->Render(GL_TRIANGLES);
		glBindTexture(GL_TEXTURE_2D, 0);




		glUseProgram(shader->GetProgram());
		glBindTexture(GL_TEXTURE_2D, eightBall->getTextureID());
		glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
		glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, arrayOfSpheres[1]->GetModelMatrix());
		sphere4Mesh->Render(GL_TRIANGLES);
		glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_CULL_FACE);
	glUseProgram(planeShader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, planeText->getTextureID());
	glUniformMatrix4fv(planeShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(planeShader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(planeShader->GetUniformID("planeModelMatrix"), 1, GL_FALSE, plane->GetModelMatrix());

	glUniform3fv(planeShader->GetUniformID("lightPosition[0]"), 5, *lightPosition);
	glUniform4fv(planeShader->GetUniformID("ks[0]"), 5, *ks);
	glUniform4fv(planeShader->GetUniformID("kd[0]"), 5, *kd);

	planeMesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

}




