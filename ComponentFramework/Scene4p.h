#ifndef Scene4p_H
#define Scene4p_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include "Trackball.h"
#include <vector>
#include "Camera.h"
#include <Triangle.h>
#include <TMath.h>
#include <Dot.h>
#include "Timing.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene4p : public Scene {
private:
	Body* triangle;
	MATHEX::Triangle* triangleShape;

	Mesh* triangleMesh;
	// Don't need the modelMatrices I talked about in the assignment doc

	Body* pointOnPlane;
	Body* pointOnLine01; // Point on line going through vertices v0 and v1
	Body* pointOnLine12; // vertices v1 and v2
	Body* pointOnLine20; // vertices v2 and v0
	Body* collisionPoint;

	Vec3 impulseDirCameraSpace = Vec3(0, 0, -1);
	Vec3 impulseDirWorldSpace;

	Body* sphere;
	Mesh* mesh;

	Shader* shader;
	
	Texture* texture;

	Body* knot;
	Mesh* knot_mesh;
	std::vector<MATHEX::Triangle*> knotTris;
	std::vector<Mesh*> triangles_from_knot_meshs;

	Trackball trackball;
	// Don't tell Scott about this
	// but lets believe in cameras for a little bit
	Vec3	   cameraPos;
	Quaternion cameraOri;

	/// Added by Scott
	Shader* drawNormalsShader;
	
	bool drawNormals;

	Camera* camera;
	

	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	bool drawInWireMode;
	int zoom = 0;

public:
	explicit Scene4p();
	virtual ~Scene4p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
	/// The w component holds the scale of the normal
	void DrawNormals(const Vec4 color) const;


};


#endif // Scene4p_H
