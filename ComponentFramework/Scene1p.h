#ifndef Scene1p_H
#define Scene1p_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include "Texture.h"
#include "Plane.h"
#include "Camera.h"
#include "Trackball.h"
#include "Collision.h"

using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene1p : public Scene {
private:
	Body* sphere;
	Body* sphere2;
	Body* arrayOfSpheres[3];
	Body* plane;



	Shader* shader;
	Shader* planeShader;



	Mesh* mesh;
	Mesh* sphere2Mesh;
	Mesh* sphere3Mesh;
	Mesh* sphere4Mesh;
	Mesh* planeMesh;


	Plane* planeShape;


	Quaternion rotation;

	Vec3 cameraPos;               // We are in 3D now, so we need the position & orientation of the camera    
	Quaternion cameraOrientation; // I love quaternions. Who wants to learn about them early?

	Vec3 impulseDirCameraSpace = Vec3(0, 0, -1);
	Vec3 impulseDirWorldSpace;
	Vec3 Movement;

	Quaternion plane1_rotationHolder, plane2_rotationHolder, plane3_rotationHolder;

	Texture* texture;
	Texture* planeText;
	Texture* eightBall;

	Vec3 lightPos;
	Vec3 rVector = Vec3(1.0f, 1.0f, 1.0f);

	Vec3 lightPosition[5];
	Vec4 ks[5];  // Specular coefficient
	Vec4 kd[5]; // Diffuse coefficient

	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 modelMatrix;
	Matrix4 planeModelMatrix;

	Camera* camera;

	Trackball trackball;

	bool drawInWireMode;
	bool move;
	bool punch = false;
	int xm, ym;
	float power = 0.0f;
	bool ballOnPlane1 = true;
	bool Sphere2OnPlane1 = true;
	bool Sphere3OnPlane1 = true;
	bool Sphere4OnPlane1 = true;

	float planeAngle = 0, planeAngle_2 = 0, planeAngle_3 = 0;
	float planeAngleX = 0, planeAngle_2X = 0, planeAngle_3X = 0;
public:
	bool swap;
	void resetAll();
	explicit Scene1p();
	virtual ~Scene1p();
	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // Scene1p_H
