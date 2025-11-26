#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <MMath.h>
#include "Scene1g.h"
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Texture.h"

#include "MemoryMonitor.h"

Scene1g::Scene1g() : mario(nullptr), shader(nullptr), mesh(nullptr),
drawInWireMode(false), texture(nullptr), leftEye(nullptr), leftEyeShader(nullptr), leftEyeMesh(nullptr),
leftEyeTexture(nullptr), rightEye(nullptr), rightEyeShader(nullptr), rightEyeMesh(nullptr),
rightEyeTexture(nullptr) {
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene1g::~Scene1g() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}

bool Scene1g::OnCreate() {
	Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);
	std::cout << "---------------------------------------------------\n";
	mario = new Body();
	mario->OnCreate();
	//lightPos = Vec3(10.0f, 0.0f, 0.0f);

	rightEye = new Body();
	rightEye->OnCreate();
	rightEye->size *= 0.25f;
	rightEye->pos = Vec3(-0.65f, 0.15f, 0.5f);
	rightEye->orientation = QMath::angleAxisRotation(-90, Vec3(0.0f, 1.0f, 0.0f));

	leftEye = new Body();
	leftEye ->OnCreate();
	leftEye->size *= 0.25f;
	leftEye->pos = Vec3(0.60f, 0.15f, 0.5f);
	leftEye->orientation = QMath::angleAxisRotation(-90, Vec3(0.0f, 1.0f, 0.0f));

	mesh = new Mesh("meshes/Skull.obj");
	mesh->OnCreate();

	rightEyeMesh = new Mesh("meshes/Sphere.obj");
	rightEyeMesh->OnCreate();

	leftEyeMesh = new Mesh("meshes/Sphere.obj");
	leftEyeMesh->OnCreate();

	texture = new Texture();
	texture->LoadImage("textures/Skull_texture.png");

	rightEyeTexture = new Texture();
	rightEyeTexture->LoadImage("textures/evilEye.png");

	leftEyeTexture = new Texture();
	leftEyeTexture->LoadImage("textures/evilEye.png");

	camera = new Camera();
	camera->OnCreate();

	shader = new Shader("shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	rightEyeShader = new Shader("shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (rightEyeShader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	leftEyeShader = new Shader("shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (leftEyeShader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}
	

	lightPosition[0] = Vec3(-10.0f, 10.0f, -5.0f); // top left it is also ks/kd[0] so red
	lightPosition[1] = Vec3(-10.0f, -10.0f, -5.0f); // bottom left it is also ks/kd[1] so green
	lightPosition[2] = Vec3(10.0f, -10.0f, -5.0f); // bottom right it is also ks/kd[2] so blue
	lightPosition[3] = Vec3(10.0f, 10.0f, -5.0f); // top right it is also ks/kd[3] so orangeish yellow
	lightPosition[4] = Vec3(0.0f, -10.0f, -5.0f); // bottom it is also ks/kd[4] so a bit purple

	ks[0] = Vec4(0.4, 0.1, 0.1, 1.0);  // Soft red highlight
	ks[1] = Vec4(0.1, 0.4, 0.1, 1.0);  // Soft green highlight
	ks[2] = Vec4(0.1, 0.1, 0.4, 1.0);  // Soft blue highlight
	ks[3] = Vec4(0.4, 0.4, 0.1, 1.0);  // Warm yellow
	ks[4] = Vec4(0.8, 0.1, 0.8, 1.0);  // Magenta touch

	kd[0] = Vec4(0.7, 0.3, 0.3, 1.0);  // Soft red-pink
	kd[1] = Vec4(0.3, 0.7, 0.3, 1.0);  // Soft green
	kd[2] = Vec4(0.3, 0.3, 0.7, 1.0);  // Soft blue
	kd[3] = Vec4(0.8, 0.5, 0.2, 1.0);  // Subdued orange
	kd[4] = Vec4(0.8, 0.3, 0.8, 1.0);  // Gentle purple


																//0.5f, 100.0f
	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 1.0f, 1000.0f);
	camera->GetViewMatrix() = MMath::lookAt(Vec3(0.0f, 0.0f, 10.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0))) * MMath::scale(0.5f, 0.5f, 0.5f);
	mario->pos = Vec3(0.0f, 0.0f, -5.0f);
	cameraOrientation = trackball.getQuat();
	return true;
}

void Scene1g::OnDestroy() {
	Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);
	mario->OnDestroy();
	delete mario;
	mario = nullptr;

	mesh->OnDestroy();
	delete mesh;
	mesh = nullptr;

	shader->OnDestroy();
	delete shader;
	shader = nullptr;

	texture->OnDestroy();
	delete texture;
	texture = nullptr;

	rightEye->OnDestroy();
	delete rightEye;
	rightEye = nullptr;

	rightEyeMesh->OnDestroy();
	delete rightEyeMesh;
	rightEyeMesh = nullptr;

	rightEyeShader->OnDestroy();
	delete rightEyeShader;
	rightEyeShader = nullptr;

	rightEyeTexture->OnDestroy();
	delete rightEyeTexture;
	rightEyeTexture = nullptr;

	leftEye->OnDestroy();
	delete leftEye;
	leftEye = nullptr;

	leftEyeMesh->OnDestroy();
	delete leftEyeMesh;
	leftEyeMesh = nullptr;

	leftEyeShader->OnDestroy();
	delete leftEyeShader;
	leftEyeShader = nullptr;

	leftEyeTexture->OnDestroy();
	delete leftEyeTexture;
	leftEyeTexture = nullptr;

	camera->OnDestroy();
	delete camera;
	camera = nullptr;


}

void Scene1g::HandleEvents(const SDL_Event& sdlEvent) {
	trackball.HandleEvents(sdlEvent);
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_P:
			mario->size.x += 0.25f;
			mario->size.y += 0.25f;
			mario->size.z += 0.25f;
			break;
		case SDL_SCANCODE_M:
			mario->size.x -= 0.25f;
			mario->size.y -= 0.25f;
			mario->size.z -= 0.25f;
			break;
		
	case SDL_SCANCODE_1:
		buttonDown = 1;
		break;
	case SDL_SCANCODE_0:
		buttonDown = 0;
		break;
		}
	case SDL_MOUSEMOTION:
		break;

	case SDL_MOUSEBUTTONDOWN:

		break;

	case SDL_MOUSEBUTTONUP:

		break;

	default:
		break;
	}
}

void Scene1g::Update(const float deltaTime) {
	
	Vec3 offset = Vec3(0.0f, 0.0f, 20.0f);
	Vec3 rotatedOffset = QMath::rotate(offset, cameraOrientation);
	cameraPos = mario->pos + rotatedOffset;
	camera->SetView(cameraOrientation, cameraPos);
	// also if you press P it will increase size 
	// press M to decrease size
	// press 1 to activate
	if (buttonDown == 1) {
		leftEye->orientation = trackball.getQuat() * QMath::angleAxisRotation(-90, Vec3(0.0f, 1.0f, 0.0f));
		rightEye->orientation = trackball.getQuat() * QMath::angleAxisRotation(-90, Vec3(0.0f, 1.0f, 0.0f));
	}
	// press 0 to activate
	else {
		Vec3 leftEyeDir = VMath::normalize(cameraPos - leftEye->pos);
		Vec3 rightEyeDir = VMath::normalize(cameraPos - rightEye->pos);
		Vec3 up = Vec3(0.0f, 1.0f, 0.0f);

		mario->orientation = trackball.getQuat();
		leftEye->orientation = QMath::inverse(trackball.getQuat()) * QMath::angleAxisRotation(-90, Vec3(0.0f, 1.0f, 0.0f));
		rightEye->orientation = QMath::inverse(trackball.getQuat()) * QMath::angleAxisRotation(-90, Vec3(0.0f, 1.0f, 0.0f));
	}
	mario->Update(deltaTime);
}

void Scene1g::Render() const {
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
	//skybox

	camera->RenderSkyBox();
	// Restore OpenGL state
	glDepthMask(GL_TRUE);  
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	//Skull
	glUseProgram(shader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, mario->GetModelMatrix());
	glUniform3fv(shader->GetUniformID("lightPosition[0]"), 5, *lightPosition);
	glUniform4fv(shader->GetUniformID("ks[0]"), 5, *ks);
	glUniform4fv(shader->GetUniformID("kd[0]"), 5, *kd);
	mesh->Render(GL_TRIANGLES);
	glDisable(GL_CULL_FACE);
	//right eye
	glUseProgram(rightEyeShader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, rightEyeTexture->getTextureID());
	glUniformMatrix4fv(rightEyeShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(rightEyeShader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(rightEyeShader->GetUniformID("modelMatrix"), 1, GL_FALSE, mario->GetModelMatrix() * rightEye->GetModelMatrix());
	glUniform3fv(rightEyeShader->GetUniformID("lightPosition[0]"), 5, *lightPosition);
	glUniform4fv(rightEyeShader->GetUniformID("ks[0]"), 5, *ks);
	glUniform4fv(rightEyeShader->GetUniformID("kd[0]"), 5, *kd);
	rightEyeMesh->Render(GL_TRIANGLES);
	glDisable(GL_CULL_FACE);
	//left eye
	glUseProgram(leftEyeShader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, leftEyeTexture->getTextureID());
	glUniformMatrix4fv(leftEyeShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(leftEyeShader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(leftEyeShader->GetUniformID("modelMatrix"), 1, GL_FALSE, mario->GetModelMatrix() * leftEye->GetModelMatrix());
	glUniform3fv(leftEyeShader->GetUniformID("lightPosition[0]"), 5, *lightPosition);
	glUniform4fv(leftEyeShader->GetUniformID("ks[0]"), 5, *ks);
	glUniform4fv(leftEyeShader->GetUniformID("kd[0]"), 5, *kd);
	leftEyeMesh->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}



