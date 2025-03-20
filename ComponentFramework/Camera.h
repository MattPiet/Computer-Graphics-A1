#pragma once
/// The camera still doesn't exist mathamatically but there is utility in placeing the
/// view and projections matricies in class. Might as well call it a camera.
#include <Vector.h>
#include <Matrix.h>
#include <Quaternion.h>
#include <MMath.h>
#include "SkyBox.h"
#include "Trackball.h"
#include "Texture.h"
using namespace MATH;

union SDL_Event;

class Camera {
private:
	Quaternion orientation;
	Vec3 position;
	Matrix4 projection;
	Matrix4 view;
	Trackball trackball;
	SkyBox* skybox;
	Vec3 target = Vec3(0.0f, 0.0f, 0.0f);
public:
	Camera();
	~Camera();
	bool OnCreate();
	void OnDestroy();
	void RenderSkyBox() const;
	void HandelEvents(const SDL_Event& sdlEvent);
	Texture* getSkyBoxTexture() const {return skybox->GetTexture(); }
	/// Some getters and setters
	Matrix4 GetViewMatrix() const {
		//return MMath::inverse(MMath::translate(position)) * MMath::inverse(MMath::toMatrix4(orientation));
		return MMath::inverse(MMath::toMatrix4(orientation)) * MMath::inverse(MMath::translate(position));
		
	}
	Matrix4 GetViewMatrix2() const {

			const auto center_point = Vec3(0.0f, 0.0f, 0.0f);
			Vec3 translated_position = center_point - target;

			return MMath::translate(position) *
				MMath::toMatrix4(orientation) *
				MMath::translate(translated_position);
		
	}

	Matrix4 GetProjectionMatrix() const {
		return projection;
	}
	void dontTrackY();
	Quaternion GetOrientation() const { return orientation; }
	void setSkyBox(std::vector<std::string> skybox_textures);
	void SetView(const Quaternion& orientation_, const Vec3& position_);
	void setTarget(Vec3 target_) {  target = target_; }

};
