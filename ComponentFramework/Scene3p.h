#ifndef SCENE3P_H
#define SCENE3P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include "Trackball.h"
#include <vector>
#include "Camera.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene3p : public Scene {
private:
	Body* jellyfishHead;
	Body* sphere;
	// Don't need the modelMatrices I talked about in the assignment doc

	// Smaller spheres just to show us where the tentacles are anchored 
	std::vector<Body*> anchors;

	// And the smallest spheres will make up the tentacles 
	std::vector<Body*> tentacleSpheres;

	std::vector<Matrix4> modelMatricesTentacleSphere;
	std::vector<Matrix4> modelMatricesAnchors;
	// These variables will tells us how long each tentacle will be  
	// and the spacing between individual spheres 
	const int numSpheresPerAnchor = 100;
	const float spacing = 1.0f;

	Shader* shader;
	Shader* sphereShader;
	Texture* texture;
	Texture* sphereTexture;
	Trackball trackball;
	// Don't tell Scott about this
	// but lets believe in cameras for a little bit
	Vec3	   cameraPos;
	Quaternion cameraOri;

	/// Added by Scott
	Shader* drawNormalsShader;
	bool drawNormals; 

	Camera* camera;
	Mesh* mesh;
	Mesh* sphereMesh;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	bool drawInWireMode;
	int tenticleIndex = 0;
	int anchorIndex = 0;
public:
	explicit Scene3p();
	virtual ~Scene3p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
	/// The w component holds the scale of the normal
	void DrawNormals(const Vec4 color) const;
};


#endif // SCENE3P_H