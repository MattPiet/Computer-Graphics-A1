#ifndef SCENE0P_H
#define SCENE0P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include "Texture.h"
#include "Plane.h"
#include "Camera.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene0p : public Scene {
private:
	Body* sphere;
	Body* plane; 
	Body* plane_2;
	Body* plane_3;


	Shader* shader;
	
	Shader* planeShader;
	Shader* planeShader_2;
	Shader* planeShader_3;


	Mesh* mesh;
	Mesh* planeMesh;
	Mesh* planeMesh_2;
	Mesh* planeMesh_3;


	surface::Plane* planeShape;
	surface::Plane* planeShape_2;
	surface::Plane* planeShape_3;

	Quaternion rotation;
	Quaternion rotationForPlane2;
	Quaternion rotationForPlane3;
	Vec3 cameraPos;               // We are in 3D now, so we need the position & orientation of the camera    
	Quaternion cameraOrientation; // I love quaternions. Who wants to learn about them early?

	Quaternion plane1_rotationHolder, plane2_rotationHolder, plane3_rotationHolder;

	Texture* texture;
	Texture* planeText;
	Texture* planeText_2;
	Texture* planeText_3;


	Vec3 planeNormal;
	Vec3 planeNormal_2;
	Vec3 planeNormal_3;
	Vec3 lightPos;
	Vec3 rVector = Vec3(1.0f,1.0f,1.0f);

	Vec3 lightPosition[5];
	Vec4 ks[5];  // Specular coefficient
	Vec4 kd[5]; // Diffuse coefficient

	Vec3 Plane_lightPosition[5];
	Vec4 Plane_ks[5];  // Specular coefficient
	Vec4 Plane_kd[5]; // Diffuse coefficient


	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 modelMatrix;
	Matrix4 planeModelMatrix;
	Matrix4 planeModelMatrix_2;
	Matrix4 planeModelMatrix_3;

	Camera* camera;
	Trackball trackball;

	bool drawInWireMode;
	bool move;
	int xm, ym;
	bool ballOnPlane1 = true;
	bool ballOnPlane2 = false;
	bool ballOnPlane3 = false;
	float planeAngle = 0, planeAngle_2 = 0, planeAngle_3 = 0;
	float planeAngleX = 0, planeAngle_2X = 0, planeAngle_3X = 0;
public:
	bool swap;
	void resetAll();
	explicit Scene0p();
	virtual ~Scene0p();
	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};


#endif // SCENE0P_H