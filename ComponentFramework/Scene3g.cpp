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
	lightPos = Vec3(0.0f, 10.0f, 5.0f);
	terrain = new Body();
	terrain->OnCreate();

	mesh = new Mesh("meshes/Sub.obj");
	mesh->OnCreate();


	texture = new Texture();
	texture->LoadImage("textures/subTexture.png");

	terrain_height_map = new Texture();
	terrain_height_map->LoadImage("textures/terrainHeight.png");

	terrain_normal_map = new Texture();
	terrain_normal_map->LoadImage("textures/terrainNormal.png");


	terrain_diffuse_map = new Texture();
	terrain_diffuse_map->LoadImage("textures/terrainDiffuse.png");


	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	std::cout << "Max supported patch verticies&d\n" << MaxPatchVertices << std::endl;
	glPatchParameteri(GL_PATCH_VERTICES, 3);

	terrainMesh = new Mesh("meshes/Plane.obj");
	terrainMesh->OnCreate();

	terrain->pos = Vec3(0.0f, -10.0f, 0.0f);
	terrain->orientation *= QMath::angleAxisRotation(-90, Vec3(1.0f, 0.0f, 0.0f));
	terrain->size = Vec3(30.0f, 30.0f, 30.0f);


	camera = new Camera();
	camera->OnCreate();

	shader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}
	tessShader = new Shader("shaders/Tessillation Vert.glsl", "shaders/Tessillation frag.glsl", 
		"shaders/Tessillation control.glsl", "shaders/Tessillation evaluator.glsl");
	if (tessShader->OnCreate() == false) {
		std::cout << "Tess Shader failed ... we have a problem\n";
	}
	std::vector<std::string> skyTexts = {
	"textures/Underwater Box_Posx.png",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X 
	"textures/Underwater Box_Negx.png",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	"textures/Underwater Box_Posy.png",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	"textures/Underwater Box_Negy.png",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	"textures/Underwater Box_Posz.png",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z// this is technically the back side but because insides are
	"textures/Underwater Box_Negz.png"
	};					// flipped when looking inside a skybox  i need to make it the front side the same applies with the back jpg

	////// -> CN TOWER
	//std::vector<std::string> skyTexts = {
	//"textures/posx.jpg",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X 
	//"textures/negx.jpg",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	//"textures/posy.jpg",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	//"textures/negy.jpg",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	//"textures/posz.jpg",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z// this is technically the back side but because insides are
	//"textures/negz.jpg"
	//};					// flipped when looking inside a skybox  i need to make it the front side the same applies with the back jpg


	camera->setSkyBox(skyTexts);

	//0.5f, 100.0f

	sub->pos = Vec3(0.0f, 0.0f, 0.0f);
	sub->size *= 4;
	//camera->dontTrackY();

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
	
	terrain_height_map->OnDestroy();
	delete terrain_height_map;

	terrain_normal_map->OnDestroy();
	delete terrain_normal_map;

	terrain_diffuse_map->OnDestroy();
	delete terrain_diffuse_map;

	terrain->OnDestroy();
	delete terrain;

	terrainMesh->OnDestroy();
	delete terrainMesh;

	tessShader->OnDestroy();
	delete tessShader;

	camera->OnDestroy();
	delete camera;


}

void Scene3g::HandleEvents(const SDL_Event& sdlEvent) {
	//trackball.HandleEvents(sdlEvent);
	camera->HandelEvents(sdlEvent);
	const Uint8* state = SDL_GetKeyboardState(NULL);



	switch (sdlEvent.type) {
		
	case SDL_MOUSEWHEEL:
		if (sdlEvent.wheel.y > 0) {
			zoom -= 2;  // Scroll up
		}
		else if (sdlEvent.wheel.y < 0) {
			zoom += 2;  // Scroll down
		}
		break;
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_P:
			height_scale++;
			break;
		case SDL_SCANCODE_M:
			//if (height_scale >= 1.0f) {
				height_scale--;
			//}
			break;
		}
		if (SDL_GetModState() & KMOD_LSHIFT) {
			switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_UP:
				if (tessLevelOuter < 32) {
					tessLevelOuter++;
				}
				break;
			case SDL_SCANCODE_T:
				if (tessLevelInner < 32 && tessLevelOuter < 32) {
					tessLevelInner++;
					tessLevelOuter++;
				}
				break;
			case SDL_SCANCODE_DOWN:
				tessLevelOuter--;
				break;
			}
		}
		if (SDL_GetModState() & KMOD_RSHIFT) {
			switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_UP:
				if (tessLevelInner < 32) {
					tessLevelInner++;
				}
				break;

			case SDL_SCANCODE_DOWN:
				tessLevelInner--;
				break;
			}
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

	Vec3 offset = Vec3(0.0f, 30.0f, 100.0f + zoom);
	Vec3 rotatedOffset = QMath::rotate(offset, camera->GetOrientation());
	cameraPos = Vec3(0.0f,0.0f,0.0f) + rotatedOffset;
	camera->SetView(camera->GetOrientation(), cameraPos);
}


void Scene3g::Render() const {
	glEnable(GL_DEPTH_TEST);
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	camera->RenderSkyBox();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(shader->GetProgram());
	//glBindTexture(GL_TEXTURE_CUBE_MAP, camera->getSkyBoxTexture()->getTextureID());

	glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sub->GetModelMatrix());

	//Vec3 Forward = Vec3(0.0f, 0.0f, -1.0f);
	//Vec3 rotatedForward = QMath::rotate(Forward, camera->GetOrientation());
	//float offset = 15;
	//Vec3 cameraPos2 = Vec3(rotatedForward.x, rotatedForward.y, -rotatedForward.z) * offset;


	//glUniform3fv(shader->GetUniformID("cameraPos"), 1, cameraPos);
	mesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(tessShader->GetProgram());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrain_height_map->getTextureID());
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrain_normal_map->getTextureID());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, terrain_diffuse_map->getTextureID());

	glUniformMatrix4fv(tessShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(tessShader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(tessShader->GetUniformID("modelMatrix"), 1, GL_FALSE, terrain->GetModelMatrix());
	glUniform1f(tessShader->GetUniformID("tessLevelOuter"), tessLevelOuter);
	glUniform1f(tessShader->GetUniformID("tessLevelInner"), tessLevelInner);
	glUniform1f(tessShader->GetUniformID("height_scale"), height_scale);
	glUniform4fv(tessShader->GetUniformID("lightPosition"), 1, lightPos);
	terrainMesh->Render(GL_PATCHES);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glUseProgram(0);
}
