#ifndef Scene3g_H
#define Scene3g_H
#include "Scene.h"
#include <Vector.h>
#include <Matrix.h>
#include "Texture.h"
#include "Light.h"
#include "Trackball.h"
#include "Camera.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;
class Texture;

class Scene3g : public Scene {
private:
	Body* sub;
	Body* terrain;
	Shader* shader;
	Shader* tessShader;
	Mesh* mesh;
	Mesh* terrainMesh;
	Texture* texture;
	Texture* terrainTexture;

	Texture* terrain_height_map;
	float height_scale = 1;

	float direction = 2.5f;

	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 modelMatrix;
	Matrix4 terrainModelMatrix;


	bool drawInWireMode;
	float rotate;

	float tessLevelOuter = 10.0f;
	float tessLevelInner = 10.0f;

	Trackball trackball;

	bool buttonDown = 1;

	Camera* camera;
	Vec3 cameraPos = Vec3(0.0f, 0.0f, -1.0f);               // We are in 3D now, so we need the position & orientation of the camera  
	Vec3 renderCameraPos = Vec3(0.0f, 0.0f, -1.0f);
	Quaternion cameraOrientation; // I love quaternions. Who wants to learn about them early?

public:
	//Quaternion rotation = QMath::angleAxisRotation(10.0f, Vec3(0.0f, 1.0f, 0.0f));
	explicit Scene3g();
	virtual ~Scene3g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // Scene3g_H

