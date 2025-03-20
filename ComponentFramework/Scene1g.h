#ifndef SCENE1G_H
#define SCENE1G_H
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

class Scene1g : public Scene {
private:
	Body* mario;
	Shader* shader;
	Mesh* mesh;
	Texture* texture;
	// right eye
	Body* rightEye;
	Shader* rightEyeShader;
	Mesh* rightEyeMesh;
	Texture* rightEyeTexture;
	// left eye
	Body* leftEye;
	Shader* leftEyeShader;
	Mesh* leftEyeMesh;
	Texture* leftEyeTexture;

	//skybox
	Shader* cubeShader;
	Mesh* Cubemesh;
	Texture* skyBox;



	Light* testLight;
	float direction = 2.5f;

	static Light* arrayOfLights[5];

	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 modelMatrix;
	Matrix4 righteyeModelMatrix;
	Matrix4 lefteyeModelMatrix;

	bool drawInWireMode;
	Vec3 lightPos;
	Vec3 lightPosition[5];
	Vec4 ks[5];  // Specular coefficient
	Vec4 kd[5]; // Diffuse coefficient
	float rotate;
	int lightDirection = 1;

	Trackball trackball;

	bool buttonDown = 1; 

	Camera* camera;
	Vec3 cameraPos;               // We are in 3D now, so we need the position & orientation of the camera    
	Quaternion cameraOrientation; // I love quaternions. Who wants to learn about them early?

public:
	//Quaternion rotation = QMath::angleAxisRotation(10.0f, Vec3(0.0f, 1.0f, 0.0f));
	explicit Scene1g();
	virtual ~Scene1g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // SCENE1G_H