#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene3p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include <QMath.h>
#include "Collision.h"

Scene3p::Scene3p() 
: jellyfishHead{nullptr}
, shader{nullptr}
, mesh{nullptr}
, drawInWireMode{false}
, drawNormals{true}
{
	Debug::Info("Created Scene3p: ", __FILE__, __LINE__);
}

Scene3p::~Scene3p() {
	Debug::Info("Deleted Scene3p: ", __FILE__, __LINE__);
}

bool Scene3p::OnCreate() {
	// Warning from Umer
	// Switch off all rolling before starting assignment 2
	Debug::Info("Loading assets Scene3p: ", __FILE__, __LINE__);
	jellyfishHead = new Body();
	jellyfishHead->OnCreate();
	jellyfishHead->radius = 6;
	jellyfishHead->size *= 6;
	jellyfishHead->pos.set(-1.5, 4, -45);

	sphere = new Body();
	sphere->OnCreate();
	sphere->radius = 3;
	sphere->size *= 3;
	sphere->pos.set(-1.5, -8, -45);
	sphere->mass = 100;

	mesh = new Mesh("meshes/Sphere.obj");
	mesh->OnCreate();

	sphereMesh = new Mesh("meshes/Sphere.obj");
	sphereMesh->OnCreate();

	texture = new Texture();
	texture->LoadImage("textures/Blank.png");

	sphereTexture = new Texture();
	sphereTexture->LoadImage("textures/8ball.png");

	camera = new Camera();
	camera->OnCreate();

		std::vector<std::string> skyTexts = {
	"textures/Underwater Box_Posx.png",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X 
	"textures/Underwater Box_Negx.png",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	"textures/Underwater Box_Posy.png",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	"textures/Underwater Box_Negy.png",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	"textures/Underwater Box_Posz.png",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z// this is technically the back side but because insides are
	"textures/Underwater Box_Negz.png"
	};					// flipped when looking inside a skybox  i need to make it the front side the same applies with the back jpg
		camera->setSkyBox(skyTexts);

	drawNormalsShader = new Shader("shaders/normalVert.glsl", "shaders/normalFrag.glsl", nullptr,nullptr,"shaders/normalGeom.glsl");
	if (drawNormalsShader->OnCreate() == false) {
		std::cout << "drawNormalsShader failed ... we have a problem\n";
	}
	shader = new Shader("shaders/NoiseVert.glsl", "shaders/mirrorFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}
	sphereShader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	if (sphereShader->OnCreate() == false) {
		std::cout << "sphere Shader failed ... we have a problem\n";
	}

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 10.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	// Let's set the pos and orientation of the camera instead of lookAt
	cameraPos = jellyfishHead->pos + Vec3(0, -5, 35);


	const int numAnchors = 10;
	Vec3 anchorPos(-6.0f, -2.0f, -45);
	for (int i = 0; i < numAnchors; i++) {
		anchors.push_back(new Body());
		anchors[i]->pos = anchorPos;
		anchors[i]->radius = 0.5f;
		anchors[i]->size *= 0.5f;
		// Move the anchor position for the next swing through this loop 
		anchorPos += Vec3(spacing, 0, 0);
			
	}
	// Lets set up the first tentacle (Umer will do this in a not so great way)
	
	
	for (int i = 0; i < numSpheresPerAnchor; i++) {
		tentacleSpheres.push_back(new Body());
		tentacleSpheres[i]->radius = 0.1f;
		tentacleSpheres[i]->size *= 0.1f;
		tentacleSpheres[i]->pos = anchors[anchorIndex]->pos - Vec3(0, spacing * tenticleIndex + 1, 0);
		tentacleSpheres[i]->mass = 1;
		tenticleIndex++;
		if (tenticleIndex == 10) {
			anchorIndex += 1;
			tenticleIndex = 0;
		}	
	}
	anchorIndex = 0;

	camera->dontTrackY();
	sphere->vel.z = 5.0f;

	lightPos = Vec3(0.0f, 10.0f, 5.0f);
	terrain = new Body();
	terrain->OnCreate();

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

	terrain->pos = Vec3(0.0f, -35.0f, 0.0f);
	terrain->orientation *= QMath::angleAxisRotation(-90, Vec3(1.0f, 0.0f, 0.0f));
	terrain->size = Vec3(30.0f, 30.0f, 30.0f);



	tessShader = new Shader("shaders/Tessillation Vert.glsl", "shaders/Tessillation frag.glsl",
		"shaders/Tessillation control.glsl", "shaders/Tessillation evaluator.glsl");
	if (tessShader->OnCreate() == false) {
		std::cout << "Tess Shader failed ... we have a problem\n";
	}

	Vec3 offset = Vec3(0.0f, 0.0f, 15.0f);
	Vec3 rotatedOffset = QMath::rotate(offset, camera->GetOrientation());
	cameraPos = Vec3(0.0f, 0.0f, 0.0f) + rotatedOffset;
	camera->SetView(camera->GetOrientation(), cameraPos);
	return true;
}

void Scene3p::OnDestroy() {
	Debug::Info("Deleting assets Scene3p: ", __FILE__, __LINE__);
	jellyfishHead->OnDestroy();
	delete jellyfishHead;

	mesh->OnDestroy();
	delete mesh;

	shader->OnDestroy();
	delete shader;

	texture->OnDestroy();
	delete texture;

	sphere->OnDestroy();
	delete sphere;

	sphereMesh->OnDestroy();
	delete sphereMesh;

	sphereShader->OnDestroy();
	delete sphereShader;

	sphereTexture->OnDestroy();
	delete sphereTexture;

	for (auto anchor : anchors) {
		anchor->OnDestroy();
		delete anchor;
	}
	
	for (auto tentacleSphere : tentacleSpheres) {
		tentacleSphere->OnDestroy();
		delete tentacleSphere;
	}

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

}

void Scene3p::HandleEvents(const SDL_Event &sdlEvent) {
	//trackball.HandleEvents(sdlEvent);
	camera->HandelEvents(sdlEvent);
	switch (sdlEvent.type) {
	case SDL_MOUSEWHEEL:
		if (sdlEvent.wheel.y > 0) {
			camera->SetView(camera->GetOrientation(), camera->freeCameraMovement(Vec3(0.0f, 0.0f, -1.0f)));
			
		}
		else if (sdlEvent.wheel.y < 0) {
			camera->SetView(camera->GetOrientation(), camera->freeCameraMovement(Vec3(0.0f, 0.0f, 1.0f)));
		}
		break;
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			jellyfishHead->pos.z--;
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.z--;
			}
			break;
		case SDL_SCANCODE_S:
			jellyfishHead->pos.z++;
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.z++;
			}
			break;
		case SDL_SCANCODE_L:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_A:
			jellyfishHead->pos.x--;
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.x--;
			}
			break;
		case SDL_SCANCODE_D:
			jellyfishHead->pos.x++;
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.x++;
			}
			break;

		case SDL_SCANCODE_N:
			if (drawNormals == false) {
				drawNormals = true;
			}
			else {
				drawNormals = false;
			}
			break;
		case SDL_SCANCODE_UP:

			camera->SetView(camera->GetOrientation(), camera->freeCameraMovement(Vec3(0.0f, 1.0f, 0.0f)));
			break;
		

		case SDL_SCANCODE_P:
			height_scale++;
			break;
		case SDL_SCANCODE_M:
			//if (height_scale >= 1.0f) {
			height_scale--;
			//}
			break;
		case SDL_SCANCODE_DOWN:

			camera->SetView(camera->GetOrientation(), camera->freeCameraMovement(Vec3(0.0f, -1.0f, 0.0f)));
			break;
		
		case SDL_SCANCODE_LEFT:
			camera->SetView(camera->GetOrientation(), camera->freeCameraMovement(Vec3(-1.0f, 0.0f, 0.0f)));
			break;
		
		case SDL_SCANCODE_RIGHT:

			camera->SetView(camera->GetOrientation(), camera->freeCameraMovement(Vec3(1.0f, 0.0f, 0.0f)));
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


void Scene3p::Update(const float deltaTime) {
	time += deltaTime;
	jellyfishHead->updatePos(deltaTime);
	cameraPos = Vec3(camera->GetViewMatrix().zero, camera->GetViewMatrix().two, camera->GetViewMatrix().three);
		for (int sphereIndex = 0; sphereIndex < 100; sphereIndex++) {
			float c = 3.5f;
			float const gravity = -5.0f;

			Vec3 gravityAcc(0.0f, gravity * tentacleSpheres[sphereIndex]->mass, 0.0f);
			Vec3 forceOfGravity = (tentacleSpheres[sphereIndex]->vel) + gravityAcc;
			Vec3 dragForce = -c * tentacleSpheres[sphereIndex]->vel;

			tentacleSpheres[sphereIndex]->ApplyForce(forceOfGravity + dragForce);

			tentacleSpheres[sphereIndex]->UpdateVel(deltaTime);
		

			if (anchorIndex == 10) {
				//std::cout << "Anchor = " << anchorIndex << std::endl;
				anchorIndex = 0;
			}
	
	
			if (tenticleIndex == 0) {
				tentacleSpheres[sphereIndex]->RodConstraint(deltaTime, anchors[anchorIndex]->pos, spacing * tenticleIndex + 1);
			}
			else {
				Vec3 restraint = tentacleSpheres[sphereIndex - 1]->pos;
				tentacleSpheres[sphereIndex]->RodConstraint(deltaTime, restraint, spacing);
			
			}

			tenticleIndex++;
			if (tenticleIndex == 10) {
				anchorIndex++;
				tenticleIndex = 0;
			}

			tentacleSpheres[sphereIndex]->updatePos(deltaTime);

		}


		
		float amplitude = 0.05f;
		float frequency = 0.5f;
		float phaseAngle = jellyfishHead->pos.x;
		Vec3 displacement;
		displacement.x = 0;
		displacement.y = amplitude * sin(frequency * time + phaseAngle);
		displacement.z = 0;
		jellyfishHead->pos += displacement;
		
		for (int i = 0; i < 10; i++) {
		anchors[i]->pos = jellyfishHead->pos;
		
		float rad = 3.5f;
		float angle = (i * 2 * M_PI) / 10;
		anchors[i]->pos = Vec3(jellyfishHead->pos.x + cos(angle) * rad,
			jellyfishHead->pos.y - 6.0f,
			jellyfishHead->pos.z + sin(angle) * rad); 
		
		}

}

void Scene3p::Render() const {
	glEnable(GL_DEPTH_TEST);
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->RenderSkyBox();

	if(drawInWireMode){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Restore OpenGL state
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(shader->GetProgram());
	//glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
	glBindTexture(GL_TEXTURE_CUBE_MAP, camera->getSkyBoxTexture()->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrixFreeCam());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, jellyfishHead->GetModelMatrix());
	glUniform1f(shader->GetUniformID("time"), time);

	//glUniform3fv(shader->GetUniformID("cameraPos"), 1, cameraPos);
	mesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, camera->getSkyBoxTexture()->getTextureID());
	for (Body* anchor : anchors) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, anchor->GetModelMatrix());
		mesh->Render(GL_TRIANGLES);
	}

	for (Body* tentacleSphere : tentacleSpheres) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, tentacleSphere->GetModelMatrix());
		mesh->Render(GL_TRIANGLES);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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
	glUniformMatrix4fv(tessShader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrixFreeCam());
	glUniformMatrix4fv(tessShader->GetUniformID("modelMatrix"), 1, GL_FALSE, terrain->GetModelMatrix());
	glUniform1f(tessShader->GetUniformID("tessLevelOuter"), tessLevelOuter);
	glUniform1f(tessShader->GetUniformID("tessLevelInner"), tessLevelInner);
	glUniform1f(tessShader->GetUniformID("height_scale"), height_scale);
	glUniform4fv(tessShader->GetUniformID("lightPosition"), 1, lightPos);
	terrainMesh->Render(GL_PATCHES);

	glActiveTexture(GL_TEXTURE0);


	/// Added by Scott
	if (drawNormals == true) {
		DrawNormals(Vec4(1.0f, 1.0f, 0.0f, 0.5f));
	}
	
		glBindTexture(GL_TEXTURE_2D, 0);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

/// Added by Scott
void Scene3p::DrawNormals(const Vec4 color) const {
	
	glUseProgram(drawNormalsShader->GetProgram());
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);
	glUniform4fv(drawNormalsShader->GetUniformID("color"), 1, color);

	//glUniformMatrix4fv(drawNormalsShader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphere->GetModelMatrix());
	//mesh->Render(GL_TRIANGLES);
	glUseProgram(0);
	
}
	
