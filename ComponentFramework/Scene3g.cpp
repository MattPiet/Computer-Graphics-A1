#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <MMath.h>
#include "Scene3g.h"
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Texture.h"

Scene3g::Scene3g() : sub(nullptr), shader(nullptr), mesh(nullptr),
drawInWireMode(false), texture(nullptr) {
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene3g::~Scene3g() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}

bool Scene3g::OnCreate() {
	Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);
	sub = new Body();
	sub->OnCreate();

	mesh = new Mesh("meshes/Sphere.obj");
	mesh->OnCreate();


	texture = new Texture();
	texture->LoadImage("textures/subTexture.png");

	terrainTexture = new Texture();
	terrainTexture->LoadImage("textures/8x8_checkered_board.png");

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	std::cout << "Max supported patch verticies&d\n" << MaxPatchVertices << std::endl;
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	terrainMesh = new Mesh("meshes/Plane.obj");
	terrainMesh->OnCreate();
	terrainModelMatrix = MMath::translate(0.0f, -10.0f, 0.0f) *
		MMath::rotate(90.0f, Vec3(1.0f, 0.0f, 0.0f)) *
		MMath::scale(40.0f, 1.0f, 40.0f);

	camera = new Camera();
	camera->OnCreate();

	shader = new Shader("shaders/mirrorVert.glsl", "shaders/mirrorFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}
	tessShader = new Shader("shaders/Tessillation Vert.glsl", "shaders/Tessillation frag.glsl", 
		"shaders/Tessillation control.glsl", "shaders/Tessillation evaluator.glsl");
	if (tessShader->OnCreate() == false) {
		std::cout << "Tess Shader failed ... we have a problem\n";
	}
	//std::vector<std::string> skyTexts = {
	//"textures/Underwater Box_Posx.png",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X 
	//"textures/Underwater Box_Negx.png",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	//"textures/Underwater Box_Posy.png",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	//"textures/Underwater Box_Negy.png",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	//"textures/Underwater Box_Posz.png",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z// this is technically the back side but because insides are
	//"textures/Underwater Box_Negz.png"
	//};					// flipped when looking inside a skybox  i need to make it the front side the same applies with the back jpg

	std::vector<std::string> skyTexts = {
	"textures/posx.jpg",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X 
	"textures/negx.jpg",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	"textures/posy.jpg",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	"textures/negy.jpg",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	"textures/posz.jpg",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z// this is technically the back side but because insides are
	"textures/negz.jpg"
	};					// flipped when looking inside a skybox  i need to make it the front side the same applies with the back jpg

	//std::vector<std::string> skyTexts = {
	//"textures/px.png",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X 
	//"textures/nx.png",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	//"textures/py.png",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	//"textures/ny.png",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	//"textures/pz.png",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z// this is technically the back side but because insides are
	//"textures/nz.png"		// ^ flipped when looking inside a skybox  i need to make it the front side the same applies with the back jpg
	//};					

	//std::vector<std::string> skyTexts = {
	//"textures/right.jpg",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X 
	//"textures/left.jpg",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	//"textures/top.jpg",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	//"textures/bottom.jpg",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	//"textures/front.jpg",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z// this is technically the back side but because insides are
	//"textures/back.jpg"
	//	};					// flipped when looking inside a skybox  i need to make it the front side the same applies with the back jpg

	camera->setSkyBox(skyTexts);

	//0.5f, 100.0f

	sub->pos = Vec3(0.0f, 0.0f, 0.0f);
	sub->size *= 4;
	camera->dontTrackY();

	return true;
}

void Scene3g::OnDestroy() {
	Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);
	sub->OnDestroy();
	delete sub;

	mesh->OnDestroy();
	delete mesh;

	shader->OnDestroy();
	delete shader;

	texture->OnDestroy();
	delete texture;


	camera->OnDestroy();
	delete camera;


}

void Scene3g::HandleEvents(const SDL_Event& sdlEvent) {
	//trackball.HandleEvents(sdlEvent);
	camera->HandelEvents(sdlEvent);
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_P:
			sub->size.x += 0.25f;
			sub->size.y += 0.25f;
			sub->size.z += 0.25f;
			break;
		case SDL_SCANCODE_M:
			sub->size.x -= 0.25f;
			sub->size.y -= 0.25f;
			sub->size.z -= 0.25f;
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

void Scene3g::Update(const float deltaTime) {
	camera->setTarget(sub->pos);
}


void Scene3g::Render() const {
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

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(shader->GetProgram());
	glBindTexture(GL_TEXTURE_CUBE_MAP, camera->getSkyBoxTexture()->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix2());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sub->GetModelMatrix());

	Vec3 Forward = Vec3(0.0f, 0.0f, -1.0f);
	Vec3 rotatedForward = QMath::rotate(Forward, camera->GetOrientation());
	float offset = 15;
	Vec3 cameraPos2 = Vec3(rotatedForward.x, rotatedForward.y, -rotatedForward.z) * offset;


	glUniform3fv(shader->GetUniformID("cameraPos"), 1, cameraPos2);
	mesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glUseProgram(tessShader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, terrainTexture->getTextureID());
	glUniformMatrix4fv(tessShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(tessShader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix2());
	glUniformMatrix4fv(tessShader->GetUniformID("modelMatrix"), 1, GL_FALSE, terrainModelMatrix);
	//glUniform1f(tessShader->GetUniformID("tessLevelOuter"), 10.0f);
	//glUniform1f(tessShader->GetUniformID("tessLevelInner"), 10.0f);
	terrainMesh->Render(GL_PATCHES);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	glUseProgram(0);
}
