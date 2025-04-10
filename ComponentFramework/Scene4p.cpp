#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene4p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include <QMath.h>
#include "Collision.h"
using namespace MATHEX;
using namespace MATH;
Scene4p::Scene4p()
	: triangle{ nullptr }
	, sphere{nullptr}
	, shader{ nullptr }
	, mesh{ nullptr }
	, drawInWireMode{ true }
	, drawNormals{ true }
	, triangleShape {nullptr}
	, pointOnLine01 {nullptr}
	, pointOnLine12 {nullptr}
	, pointOnLine20 {nullptr}
	, collisionPoint {nullptr}
	, camera {nullptr}
	, texture {nullptr}
	, triangleMesh {nullptr}

{
	Debug::Info("Created Scene4p: ", __FILE__, __LINE__);
}

Scene4p::~Scene4p() {
	Debug::Info("Deleted Scene4p: ", __FILE__, __LINE__);
}

bool Scene4p::OnCreate() {
	// Warning from Umer
	// Switch off all rolling before starting assignment 2
	Debug::Info("Loading assets Scene4p: ", __FILE__, __LINE__);
	triangle = new Body();
	triangle->OnCreate();
	triangle->pos.set(0, 0, 0);

	sphere = new Body();
	sphere->OnCreate();
	sphere->pos.set(0, 15, 5);
	sphere->radius = 1.675f;

	pointOnPlane = new Body();
	pointOnPlane->OnCreate();
	pointOnPlane->size *= 0.25;

	pointOnLine01 = new Body(); 
	pointOnLine12 = new Body(); 
	pointOnLine20 = new Body();
	pointOnLine01->OnCreate();
	pointOnLine12->OnCreate();
	pointOnLine20->OnCreate();
	pointOnLine01->size *= 0.25;
	pointOnLine12->size *= 0.25;
	pointOnLine20->size *= 0.25;

	collisionPoint = new Body();
	collisionPoint->OnCreate();

	triangleShape = new Triangle(
		Vec3(-2.0f, -1.0f, -45.0f),
		Vec3(2.0f, -1.0f, -45.0f),
		Vec3(0.0f, 3.0f, -45.0f)
	);

	knot = new Body();
	knot->OnCreate();
	//knot->pos.set(0.0f, 0.0f, 0.0f);

	knot_mesh = new Mesh("meshes/Knot.obj");
	knot_mesh->need_vertices = true;
	knot_mesh->OnCreate();
	int j = 0;
	
	for (int i = 0; i < knot_mesh->vertices.size(); i++) {
		
		if (j > knot_mesh->vertices.size() - 1) {
			break;
		}

		Triangle* newTri = new Triangle(knot_mesh->vertices[0 + j]
			, knot_mesh->vertices[1 + j]
			, knot_mesh->vertices[2 + j]);
		knotTris.push_back(newTri);
		j += 3;	
	}
	
	knot_mesh->vertices.clear();
	knot_mesh->need_vertices = false;

	//for (int i = 0; i < knotTris.size(); i++) {
	//	Mesh* new_tri_mesh = new Mesh();
	//	new_tri_mesh->OnCreate(knotTris[i]);
	//	triangles_from_knot_meshs.push_back(new_tri_mesh);
	//}

	mesh = new Mesh("meshes/Sphere.obj");
	mesh->OnCreate();

	triangleMesh = new Mesh();
	triangleMesh->OnCreate(triangleShape);


	texture = new Texture();
	texture->LoadImage("textures/Blank.png");



	camera = new Camera();
	camera->OnCreate();

	//std::vector<std::string> skyTexts = {
	//"textures/Underwater Box_Posx.png",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X 
	//"textures/Underwater Box_Negx.png",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	//"textures/Underwater Box_Posy.png",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	//"textures/Underwater Box_Negy.png",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	//"textures/Underwater Box_Posz.png",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z// this is technically the back side but because insides are
	//"textures/Underwater Box_Negz.png"
	//};					// flipped when looking inside a skybox  i need to make it the front side the same applies with the back jpg
	//camera->setSkyBox(skyTexts);

	drawNormalsShader = new Shader("shaders/normalVert.glsl", "shaders/normalFrag.glsl", nullptr, nullptr, "shaders/normalGeom.glsl");
	if (drawNormalsShader->OnCreate() == false) {
		std::cout << "drawNormalsShader failed ... we have a problem\n";
	}
	shader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}


	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 10.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	// Let's set the pos and orientation of the camera instead of lookAt
	

	//camera->dontTrackY();

	return true;
}

void Scene4p::OnDestroy() {
	Debug::Info("Deleting assets Scene4p: ", __FILE__, __LINE__);
	triangle->OnDestroy();
	delete triangle;

	sphere->OnDestroy();
	delete sphere;

	mesh->OnDestroy();
	delete mesh;

	triangleMesh->OnDestroy();
	delete triangleMesh;

	shader->OnDestroy();
	delete shader;

	texture->OnDestroy();
	delete texture;

	delete triangleShape;

	pointOnLine01->OnDestroy();
	pointOnLine12->OnDestroy();
	pointOnLine20->OnDestroy();

	delete pointOnLine01;
	delete pointOnLine12;
	delete pointOnLine20;

	collisionPoint->OnDestroy();
	delete collisionPoint;

	camera->OnDestroy();
	delete camera;

	knot_mesh->OnDestroy();
	delete knot_mesh;

	for (int i = 0; i < knotTris.size() - 1; i++) {
		delete knotTris[i];
	}

	for (int i = 0; i < triangles_from_knot_meshs.size(); i++) {
		triangles_from_knot_meshs[i]->OnDestroy();
		delete triangles_from_knot_meshs[i];
	}
}

void Scene4p::HandleEvents(const SDL_Event& sdlEvent) {
	//trackball.HandleEvents(sdlEvent);
	camera->HandelEvents(sdlEvent);
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
			sphere->pos.z--;
			break;
		case SDL_SCANCODE_S:
			sphere->pos.z++;
			break;
		case SDL_SCANCODE_L:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_A:
			sphere->pos.x--;
			break;
		case SDL_SCANCODE_D:
			sphere->pos.x++;
			break;
		case SDL_SCANCODE_SPACE:
			sphere->pos.y++;
			break;
		case SDL_SCANCODE_LSHIFT:
			sphere->pos.y--;
			break;
		case SDL_SCANCODE_G:

			sphere->vel.x = 5;

			break;
		case SDL_SCANCODE_R:
			sphere->pos.set(0, 15, 5);
			sphere->vel.set(0.0f,0.0f,0.0f);
			break;

		case SDL_SCANCODE_N:
			if (drawNormals == false) {
				drawNormals = true;
			}
			else {
				drawNormals = false;
			}
			break;
		}
		break;

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

void Scene4p::Update(const float deltaTime) {
	sphere->updatePos(deltaTime);
	sphere->UpdateVel(deltaTime);
	Vec3 offset = Vec3(0.0f, 0.0f, 80.0f + zoom);
	Vec3 rotatedOffset = QMath::rotate(offset, camera->GetOrientation());
	cameraPos = sphere->pos + rotatedOffset;
	camera->SetView(camera->GetOrientation(), cameraPos);

	for (int i = 0; i < knotTris.size(); i++) {

		Vec4 pointOnPlane4d = project(sphere->pos, TMath::getPlane(*knotTris[i]));

		pointOnPlane->pos = VMath::perspectiveDivide(pointOnPlane4d);

		DualQuat line01 = knotTris[i]->getV0() & knotTris[i]->getV1();

		Vec4 point_on_line_01_4 = project(sphere->pos, line01);
		pointOnLine01->pos = VMath::perspectiveDivide(point_on_line_01_4);

		if (VMath::distance(sphere->pos, pointOnLine01->pos) < 5.0f) {
			DualQuat line12 = knotTris[i]->getV1() & knotTris[i]->getV2();
			DualQuat line20 = knotTris[i]->getV2() & knotTris[i]->getV0();

		Vec4 point_on_line_12_4 = project(sphere->pos, line12);
		pointOnLine12->pos = VMath::perspectiveDivide(point_on_line_12_4);

		Vec4 point_on_line_20_4 = project(sphere->pos, line20);
		pointOnLine20->pos = VMath::perspectiveDivide(point_on_line_20_4);


			Body::LineCollision(sphere, pointOnPlane, knotTris[i], collisionPoint, line01, line12, line20,
				pointOnLine01, pointOnLine12, pointOnLine20);
		}
	}
	
}

void Scene4p::Render() const {
	glEnable(GL_DEPTH_TEST);
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	camera->RenderSkyBox();

	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	Vec4 colour = Vec4(1.0f, 0.0f, 1.0f, 1.0f);
	// Restore OpenGL state
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(shader->GetProgram());
	//glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
	//glBindTexture(GL_TEXTURE_CUBE_MAP, camera->getSkyBoxTexture()->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, triangle->GetModelMatrix());
	glUniform4fv(shader->GetUniformID("colour"), 1, colour);
	triangleMesh->Render(GL_TRIANGLES);

	///////////////
	//colour = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
	//for (int i = 0; i < triangles_from_knot_meshs.size(); i++) {
	//	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	//	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	//	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, triangle->GetModelMatrix());
	//	glUniform4fv(shader->GetUniformID("colour"), 1, colour);
	//	triangles_from_knot_meshs[i]->Render(GL_TRIANGLES);
	//}
	/////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glUseProgram(shader->GetProgram());
	//glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
	//glBindTexture(GL_TEXTURE_CUBE_MAP, camera->getSkyBoxTexture()->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphere->GetModelMatrix());
	glUniform4fv(shader->GetUniformID("colour"), 1, colour);
	mesh->Render(GL_TRIANGLES);

	// Knot
	colour = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, knot->GetModelMatrix());
	glUniform4fv(shader->GetUniformID("colour"), 1, colour);
	knot_mesh->Render(GL_TRIANGLES);

	//knot ^


	/// Added by Scott
	if (drawNormals == true) {
		DrawNormals(Vec4(1.0f, 1.0f, 0.0f, 0.5f));
	}

	//glBindTexture(GL_TEXTURE_2D, 0);
	
}

/// Added by Scott
void Scene4p::DrawNormals(const Vec4 color) const {

	glUseProgram(drawNormalsShader->GetProgram());
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);
	glUniform4fv(drawNormalsShader->GetUniformID("color"), 1, color);

	//glUniformMatrix4fv(drawNormalsShader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphere->GetModelMatrix());
	//mesh->Render(GL_TRIANGLES);
	glUseProgram(0);

}

